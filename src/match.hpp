#ifndef MATCH_HPP
#define MATCH_HPP

#include <cassert>
#include <fstream>
#include <future>
#include <iostream>
#include <libataxx/pgn.hpp>
#include <libataxx/uaiengine.hpp>
#include <mutex>
#include <queue>
#include <vector>

using namespace libataxx;

std::queue<std::string> positions;
std::mutex positions_mutex;
std::mutex file_mutex;

void play(std::ofstream &file) {
    auto engine1 = libataxx::uai::Engine("engines/tiktaxx");
    engine1.uai();

    auto engine2 = libataxx::uai::Engine("engines/skeleton");
    engine2.uai();

    libataxx::SearchOptions opts{.type = libataxx::SearchType::Movetime,
                                 .movetime = 10};

    while (true) {
        libataxx::Position pos;
        std::string fen;

        // Get game data
        {
            std::cout << "Start game" << std::endl;
            std::lock_guard<std::mutex> lock(positions_mutex);
            if (positions.empty()) {
                break;
            }
            fen = positions.front();
            positions.pop();
        }

        // Reset engines
        engine1.uainewgame();
        engine2.uainewgame();
        engine1.isready();
        engine2.isready();

        // Play game
        pos.set_fen(fen);
        while (!pos.gameover()) {
            libataxx::uai::Engine *engine =
                pos.turn() == libataxx::Side::Black ? &engine1 : &engine2;

            assert(engine);

            // Set position
            engine->position(pos);

            // Get move
            engine->isready();
            const auto [bestmove, pondermove] = engine->go(opts);

            // Check move
            if (!pos.legal_move(bestmove)) {
                // return;
            }

            // Make move
            pos.makemove(bestmove);
        }

        // Create pgn
        auto pgn = libataxx::pgn::Game::from_position(pos);
        pgn.header["Black"] = engine1.name();
        pgn.header["White"] = engine2.name();
        pgn.header["FEN"] = fen;
        pgn.header["Timecontrol"] = "1+0.01s";
        if (pos.result() == libataxx::Result::BlackWin) {
            pgn.header["Result"] = "1-0";
        } else if (pos.result() == libataxx::Result::WhiteWin) {
            pgn.header["Result"] = "0-1";
        } else if (pos.result() == libataxx::Result::Draw) {
            pgn.header["Result"] = "1/2-1/2";
        } else {
            pgn.header["Result"] = "*";
        }

        // Save pgn
        {
            std::lock_guard<std::mutex> lock(file_mutex);
            if (file.is_open()) {
                file << pgn;
            }
        }
    }

    engine1.quit();
    engine2.quit();
}

void match(int concurrency, int games) {
    for (int i = 0; i < games; ++i) {
        positions.push("startpos");
    }

    std::ofstream file("out.pgn");

    if (!file.is_open()) {
        return;
    }

    // Create threads
    std::vector<std::future<void>> futures;
    for (int i = 0; i < concurrency; ++i) {
        futures.push_back(std::async(std::launch::async, play, std::ref(file)));
    }

    // Wait
    for (int i = 0; i < concurrency; ++i) {
        futures[i].wait();
    }
}

#endif
