#include <chrono>
#include <iostream>
#include <libataxx/pgn.hpp>
#include <libataxx/uaiengine.hpp>
#include <mutex>
#include "game.hpp"
#include "match.hpp"
#include "settings.hpp"

using namespace std::chrono;
using namespace libataxx;
using namespace libataxx::engine;
using SearchType = libataxx::engine::SearchSettings::Type;

namespace match {

libataxx::pgn::PGN Match::play(const Settings &settings, const Game &game) {
    // Get engine & position settings
    auto pos = Position{game.fen};

    // Create PGN
    pgn::PGN pgn;
    pgn.header().add("Event", settings.pgn_event);
    pgn.header().add(settings.colour1, game.engine1.name);
    pgn.header().add(settings.colour2, game.engine2.name);
    pgn.header().add("FEN", game.fen);
    auto *node = pgn.root();

    int btime = settings.tc.btime;
    int wtime = settings.tc.wtime;
    bool out_of_time = false;
    bool illegal_move = false;
    bool engine_crash = false;
    auto result = libataxx::Result::None;

    try {
        // Start engines
        auto engine1 = engine::UAIEngine(game.engine1.path);
        auto engine2 = engine::UAIEngine(game.engine2.path);

        // Did the engines start?
        if (!engine1.running()) {
            throw "asd 1";
        }
        if (!engine2.running()) {
            throw "asd 2";
        }

        engine1.uai();
        engine2.uai();

        // Set engine options
        for (const auto &[key, val] : game.engine1.options) {
            engine1.set_option(key, val);
        }
        for (const auto &[key, val] : game.engine2.options) {
            engine2.set_option(key, val);
        }

        engine1.isready();
        engine2.isready();

        // Play
        while (!pos.gameover()) {
            auto *engine = pos.turn() == Side::Black ? &engine1 : &engine2;

            engine->position(pos);

            engine->isready();

            auto search = settings.tc;

            if (search.type == SearchType::Time) {
                if (btime <= 0 || wtime <= 0) {
                    throw "meh";
                }
            }

            // Update tc
            search.btime = btime;
            search.wtime = wtime;

            // Start move timer
            const auto t0 = high_resolution_clock::now();

            // Get move
            libataxx::Move move;
            try {
                move = engine->go(search);
            } catch (...) {
                move = libataxx::Move::nullmove();
                engine_crash = true;
            }

            // Stop move timer
            const auto t1 = high_resolution_clock::now();

            // Get move time
            const auto diff = duration_cast<milliseconds>(t1 - t0);

            // Add move to .pgn
            node = node->add_mainline(move);

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
            if (settings.tc.type == SearchType::Time) {
                if (pos.turn() == Side::Black) {
                    btime -= diff.count();
                } else {
                    wtime -= diff.count();
                }
            }

            // Out of time?
            switch (settings.tc.type) {
                case SearchType::Movetime:
                    if (diff.count() >
                        settings.tc.movetime + settings.timeout_buffer) {
                        out_of_time = true;
                        if (pos.turn() == libataxx::Side::Black) {
                            result = libataxx::Result::WhiteWin;
                        } else {
                            result = libataxx::Result::BlackWin;
                        }
                        break;
                    }
                    break;
                case SearchType::Time:
                    if (btime <= 0) {
                        out_of_time = true;
                        result = libataxx::Result::WhiteWin;
                    } else if (wtime <= 0) {
                        out_of_time = true;
                        result = libataxx::Result::BlackWin;
                    }
                    break;
                default:
                    break;
            }

            // Increments
            if (settings.tc.type == SearchType::Time) {
                if (pos.turn() == Side::Black) {
                    btime += settings.tc.binc;
                } else {
                    wtime += settings.tc.winc;
                }
            }

            // Add the time left
            if (settings.pgn_verbose && settings.tc.type == SearchType::Time) {
                if (pos.turn() == Side::Black) {
                    node->add_comment("time left " + std::to_string(btime) +
                                      "ms");
                } else {
                    node->add_comment("time left " + std::to_string(wtime) +
                                      "ms");
                }
            }

            pos.makemove(move);
        }

        // Stop engines
        engine1.quit();
        engine2.quit();
    } catch (...) {
        engine_crash = true;
        node->add_comment("engine crashed");
        if (pos.turn() == libataxx::Side::Black) {
            result = libataxx::Result::WhiteWin;
        } else {
            result = libataxx::Result::BlackWin;
        }
    }

    // Game finished normally
    if (result == libataxx::Result::None) {
        result = pos.result();
    }

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

    return pgn;
}

}  // namespace match
