#include "play.hpp"
#include <cassert>
#include <chrono>
#include <iostream>
#include <libataxx/pgn.hpp>
#include <memory>
#include <mutex>
#include <thread>
#include "../cache.hpp"
#include "../engine/engine.hpp"
#include "../engine/uaiengine.hpp"
#include "../engine/uciengine.hpp"
#include "settings.hpp"

thread_local Cache<int, std::shared_ptr<Engine>> engine_cache(2);

enum class ResultReason : int
{
    Normal = 0,
    OutOfTime,
    MaterialImbalance,
    EasyFill,
    Gamelength,
    IllegalMove,
    EngineCrash,
    None,
};

[[nodiscard]] constexpr auto make_win_for(const libataxx::Side s) noexcept {
    return s == libataxx::Side::Black ? libataxx::Result::BlackWin : libataxx::Result::WhiteWin;
}

static_assert(make_win_for(libataxx::Side::Black) == libataxx::Result::BlackWin);
static_assert(make_win_for(libataxx::Side::White) == libataxx::Result::WhiteWin);

auto info_send(const std::string &msg) noexcept -> void {
    std::cout << std::this_thread::get_id() << "> " << msg << "\n";
}

auto info_recv(const std::string &msg) noexcept -> void {
    std::cout << std::this_thread::get_id() << "< " << msg << "\n";
}

[[nodiscard]] auto make_engine(const EngineSettings &settings, const bool debug = false) -> std::shared_ptr<Engine> {
    std::shared_ptr<Engine> engine;

    switch (settings.proto) {
        case EngineProtocol::UAI:
            if (debug) {
                engine = std::make_shared<UAIEngine>(settings.path, info_send, info_recv);
            } else {
                engine = std::make_shared<UAIEngine>(settings.path);
            }
            break;
        case EngineProtocol::UCI:
            if (debug) {
                engine = std::make_shared<UCIEngine>(settings.path, info_send, info_recv);
            } else {
                engine = std::make_shared<UCIEngine>(settings.path);
            }
            break;
        default:
            throw std::invalid_argument("Unknown engine protocol");
    }

    engine->init();
    for (const auto &[key, val] : settings.options) {
        engine->set_option(key, val);
    }

    engine->isready();

    return engine;
}

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
    auto result = libataxx::Result::None;
    auto result_reason = ResultReason::None;
    auto movestr = std::string();

    // If the engines we need aren't in the cache, we get nothing
    auto engine1 = engine_cache.get(game.engine1.id);
    auto engine2 = engine_cache.get(game.engine2.id);

    // Free resources by removing any engine processes left in the cache
    engine_cache.clear();

    // Create new engine processes if necessary, knowing we have the resources available
    if (!engine1) {
        if (settings.verbose) {
            std::cout << "Create engine process " << game.engine1.name << "\n";
        }

        engine1 = make_engine(game.engine1, settings.debug);
    }

    if (!engine2) {
        if (settings.verbose) {
            std::cout << "Create engine process " << game.engine1.name << "\n";
        }

        engine2 = make_engine(game.engine2, settings.debug);
    }

    try {
        assert(engine1);
        assert(engine2);

        assert(*engine1);
        assert(*engine2);

        assert(*engine1 != *engine2);

        (*engine1)->newgame();
        (*engine2)->newgame();

        (*engine1)->isready();
        (*engine2)->isready();

        // Play
        while (!pos.gameover()) {
            // Try to adjudicate based on material imbalance
            if (settings.adjudicate_material) {
                const auto material_imbalance = pos.us().count() - pos.them().count();
                if (material_imbalance >= *settings.adjudicate_material) {
                    result = make_win_for(pos.turn());
                    result_reason = ResultReason::MaterialImbalance;
                    break;
                }
            }

            // Try to adjudicate based on "easy fill"
            // This is when one side has to pass and the other can fill the rest of the board trivially to win
            if (settings.adjudicate_easyfill && pos.must_pass()) {
                const auto them_stuck = (pos.us().singles() | pos.us().doubles()) & pos.them();
                const auto them_unstuck = pos.them() ^ them_stuck;
                assert(pos.them() == (them_stuck | them_unstuck));
                const auto their_reach =
                    pos.reachable((them_stuck.singles() | them_unstuck.doubles()) & pos.empty(), pos.empty());
                const auto all_them = pos.them() | their_reach;
                const auto is_easyfill = (all_them | pos.empty()) == all_them;
                const auto is_winning = all_them.count() > pos.us().count();
                if (is_easyfill && is_winning) {
                    result = make_win_for(!pos.turn());
                    result_reason = ResultReason::EasyFill;
                    break;
                }
            }

            // Try to adjudicate based on game length
            if (settings.adjudicate_gamelength && pos.fullmoves() >= *settings.adjudicate_gamelength) {
                result = libataxx::Result::Draw;
                result_reason = ResultReason::Gamelength;
                break;
            }

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
            movestr = engine->go(search);

            // Stop move timer
            const auto t1 = std::chrono::high_resolution_clock::now();

            // Get move time
            const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);

            libataxx::Move move;

            try {
                // Parse move string
                move = libataxx::Move::from_uai(movestr);

                // Illegal move
                if (!pos.legal_move(move)) {
                    throw;
                }
            } catch (...) {
                result_reason = ResultReason::IllegalMove;
                result = make_win_for(!pos.turn());
                std::cout << "Illegal move \"" << movestr << "\" played by "
                          << (pos.turn() == libataxx::Side::Black ? game.engine1.name : game.engine2.name) << "\n\n";
                break;
            }

            ply_count++;

            // Add move to .pgn
            node = node->add_mainline(move);

            // Comment with engine data
            if (settings.pgn_verbose) {
                node->add_comment("movetime " + std::to_string(diff.count()));
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
                    result_reason = ResultReason::OutOfTime;
                    result = make_win_for(!pos.turn());
                    break;
                }
            } else if (settings.tc.type == SearchSettings::Type::Time) {
                if (btime <= 0) {
                    result_reason = ResultReason::OutOfTime;
                    result = libataxx::Result::WhiteWin;
                    break;
                } else if (wtime <= 0) {
                    result_reason = ResultReason::OutOfTime;
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
        result_reason = ResultReason::EngineCrash;
        result = make_win_for(!pos.turn());
    }

    engine_cache.push(game.engine1.id, *engine1);
    engine_cache.push(game.engine2.id, *engine2);

    (*engine1).reset();
    (*engine2).reset();

    // Game finished normally
    if (result == libataxx::Result::None) {
        result = pos.result();
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

    switch (result_reason) {
        case ResultReason::Normal:
            break;
        case ResultReason::OutOfTime:
            pgn.header().add("Adjudicated", "Out of time");
            break;
        case ResultReason::MaterialImbalance:
            pgn.header().add("Adjudicated", "Material imbalance");
            break;
        case ResultReason::EasyFill:
            pgn.header().add("Adjudicated", "Easy fill");
            break;
        case ResultReason::Gamelength:
            pgn.header().add("Adjudicated", "Max game length reached");
            break;
        case ResultReason::IllegalMove:
            pgn.header().add("Adjudicated", "Illegal move " + movestr);
            break;
        default:
            break;
    }

    // Add some game statistics
    const auto material_difference = pos.black().count() - pos.white().count();
    pgn.header().add("PlyCount", std::to_string(ply_count));
    pgn.header().add("Final FEN", pos.get_fen());
    pgn.header().add("Material", (material_difference >= 0 ? "+" : "") + std::to_string(material_difference));

    return pgn;
}
