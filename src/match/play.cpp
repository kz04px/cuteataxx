#include "play.hpp"
#include <cassert>
#include <chrono>
#include <libataxx/pgn.hpp>
#include <memory>
#include <mutex>
#include "../cache.hpp"
#include "../uaiengine.hpp"
#include "settings.hpp"

thread_local Cache<int, std::shared_ptr<UAIEngine>> engine_cache(2);

[[nodiscard]] libataxx::pgn::PGN play(const Settings &settings, const GameSettings &game) {
    assert(!game.fen.empty());
    assert(game.engine1.id != game.engine2.id);

    // Get engine & position settings
    auto pos = libataxx::Position{game.fen};

    // Create PGN
    libataxx::pgn::PGN pgn;
    pgn.header().add("Event", settings.pgn_event);
    pgn.header().add(settings.colour1, game.engine1.name);
    pgn.header().add(settings.colour2, game.engine2.name);
    pgn.header().add("FEN", game.fen);
    auto node = pgn.root();

    int ply_count = 0;
    int btime = settings.tc.btime;
    int wtime = settings.tc.wtime;
    bool out_of_time = false;
    bool illegal_move = false;
    bool engine_crash = false;
    auto result = libataxx::Result::None;

    auto engine1 = engine_cache.get(game.engine1.id);
    if (!engine1) {
        engine1 = std::make_shared<UAIEngine>(game.engine1.path);
        (*engine1)->uai();
        for (const auto &[key, val] : game.engine1.options) {
            (*engine1)->set_option(key, val);
        }
    }

    auto engine2 = engine_cache.get(game.engine2.id);
    if (!engine2) {
        engine2 = std::make_shared<UAIEngine>(game.engine2.path);
        (*engine2)->uai();
        for (const auto &[key, val] : game.engine2.options) {
            (*engine2)->set_option(key, val);
        }
    }

    try {
        assert(engine1);
        assert(engine2);

        assert(*engine1);
        assert(*engine2);

        assert(*engine1 != *engine2);

        (*engine1)->isready();
        (*engine2)->isready();

        // Play
        while (!pos.gameover() && pos.fullmoves() < settings.maxfullmoves) {
            auto engine = pos.turn() == libataxx::Side::Black ? *engine1 : *engine2;

            engine->position(pos);

            engine->isready();

            auto search = settings.tc;

            if (search.type == SearchSettings::Type::Time) {
                if (btime <= 0 || wtime <= 0) {
                    throw "meh";
                }
            }

            // Update tc
            search.btime = btime;
            search.wtime = wtime;

            // Start move timer
            const auto t0 = std::chrono::high_resolution_clock::now();

            // Get move
            libataxx::Move move;
            try {
                move = engine->go(search);
            } catch (...) {
                move = libataxx::Move::nullmove();
                engine_crash = true;
            }

            // Stop move timer
            const auto t1 = std::chrono::high_resolution_clock::now();

            // Get move time
            const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);

            // Add move to .pgn
            node = node->add_mainline(move);

            ply_count++;

            // Comment with engine data
            if (settings.pgn_verbose) {
                node->add_comment("movetime " + std::to_string(diff.count()));
            }

            // Illegal move played
            if (!pos.legal_move(move)) {
                node->add_comment("illegal move");
                illegal_move = true;
                break;
            }

            // Engine crashed
            if (engine_crash) {
                node->add_comment("engine crashed");
                if (pos.turn() == libataxx::Side::Black) {
                    result = libataxx::Result::WhiteWin;
                } else {
                    result = libataxx::Result::BlackWin;
                }
                break;
            }

            // Update clock
            if (settings.tc.type == SearchSettings::Type::Time) {
                if (pos.turn() == libataxx::Side::Black) {
                    btime -= diff.count();
                } else {
                    wtime -= diff.count();
                }
            }

            // Out of time?
            if (settings.tc.type == SearchSettings::Type::Movetime) {
                if (diff.count() > settings.tc.movetime + settings.timeout_buffer) {
                    out_of_time = true;
                    if (pos.turn() == libataxx::Side::Black) {
                        result = libataxx::Result::WhiteWin;
                    } else {
                        result = libataxx::Result::BlackWin;
                    }
                    break;
                }
            } else if (settings.tc.type == SearchSettings::Type::Time) {
                if (btime <= 0) {
                    out_of_time = true;
                    result = libataxx::Result::WhiteWin;
                    break;
                } else if (wtime <= 0) {
                    out_of_time = true;
                    result = libataxx::Result::BlackWin;
                    break;
                }
            }

            // Increments
            if (settings.tc.type == SearchSettings::Type::Time) {
                if (pos.turn() == libataxx::Side::Black) {
                    btime += settings.tc.binc;
                } else {
                    wtime += settings.tc.winc;
                }
            }

            // Add the time left
            if (settings.pgn_verbose && settings.tc.type == SearchSettings::Type::Time) {
                if (pos.turn() == libataxx::Side::Black) {
                    node->add_comment("time left " + std::to_string(btime) + "ms");
                } else {
                    node->add_comment("time left " + std::to_string(wtime) + "ms");
                }
            }

            pos.makemove(move);
        }

    } catch (...) {
        engine_crash = true;
        node->add_comment("engine crashed");
        if (pos.turn() == libataxx::Side::Black) {
            result = libataxx::Result::WhiteWin;
        } else {
            result = libataxx::Result::BlackWin;
        }
    }

    engine_cache.push(game.engine1.id, *engine1);
    engine_cache.push(game.engine2.id, *engine2);

    (*engine1).reset();
    (*engine2).reset();

    // Illegal move
    if (illegal_move) {
        pgn.header().add("Adjudicated", "Illegal move");
    }
    // Out of time
    else if (out_of_time) {
        pgn.header().add("Adjudicated", "Out of time");
    }
    // Engine crashed
    else if (engine_crash) {
        pgn.header().add("Adjudicated", "Engine crashed");
    }
    // Game finished normally
    else if (pos.gameover()) {
        result = pos.result();
    }
    // Max fullmove counter was hit
    else if (pos.fullmoves() >= settings.maxfullmoves) {
        result = libataxx::Result::Draw;
        pgn.header().add("Adjudicated", "Max game length reached");
    }

    // Add result to .pgn
    switch (result) {
        case libataxx::Result::BlackWin:
            pgn.header().add("Result", "1-0");
            break;
        case libataxx::Result::WhiteWin:
            pgn.header().add("Result", "0-1");
            break;
        case libataxx::Result::Draw:
            pgn.header().add("Result", "1/2-1/2");
            break;
        default:
            pgn.header().add("Result", "*");
            break;
    }

    // Add some game statistics
    const auto material_difference = pos.black().count() - pos.white().count();
    pgn.header().add("PlyCount", std::to_string(ply_count));
    pgn.header().add("Final FEN", pos.get_fen());
    pgn.header().add("Material", (material_difference >= 0 ? "+" : "") + std::to_string(material_difference));

    return pgn;
}
