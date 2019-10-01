#include <chrono>
#include <iostream>
#include <libataxx/pgn.hpp>
#include <libataxx/uaiengine.hpp>
#include <mutex>
#include "match.hpp"

using namespace std::chrono;
using namespace libataxx;
using namespace libataxx::engine;

void Match::play() noexcept {
    while (true) {
        Game game;

        // Get something to do
        {
            std::lock_guard<std::mutex> lock(mtx_games_);

            // We ran out of work to do
            if (games_.empty()) {
                return;
            }

            game = games_.front();
            games_.pop();
        }

        // Get engine & position settings
        auto pos = Position{game.fen};
        auto &settings1 = settings_.engines.at(game.name1);
        auto &settings2 = settings_.engines.at(game.name2);

        // Start engines
        auto engine1 = engine::UAIEngine(settings1.path);
        auto engine2 = engine::UAIEngine(settings2.path);

        // Did the engines start?
        if (!engine1.running() || !engine2.running()) {
            if (!engine1.running()) {
                scores_.at(game.name1).crashes++;
            }

            if (!engine2.running()) {
                scores_.at(game.name2).crashes++;
            }

            continue;
        }

        engine1.uai();
        engine1.isready();
        engine2.uai();
        engine2.isready();

        /*
        // Set engine options
        engine1.setoption("hash", 128);
        engine2.setoption("hash", 128);
        */

        // Create PGN
        pgn::PGN pgn;
        pgn.header().add("Black", game.name1);
        pgn.header().add("White", game.name2);
        pgn.header().add("FEN", game.fen);

        auto *node = pgn.root();

        // Play
        while (!pos.gameover()) {
            auto *engine = pos.turn() == Side::Black ? &engine1 : &engine2;

            engine->position(pos);

            engine->isready();

            auto search = SearchSettings{.type = SearchSettings::Type::Movetime,
                                         .movetime = 10};

            const auto move = engine->go(search);

            // Add move to .pgn
            node = node->add_mainline(move);

            // Illegal move played
            if (!pos.legal_move(move)) {
                pgn.header().add("Adjudicated", "Illegal move");
                break;
            }

            pos.makemove(move);
        }

        // Stop engines
        engine1.quit();
        engine2.quit();

        // Results & printing
        {
            std::lock_guard<std::mutex> lock(mtx_output_);
            games_completed_++;

            // Update engine results
            switch (pos.result()) {
                case Result::BlackWin:
                    scores_.at(game.name1).wins++;
                    scores_.at(game.name2).losses++;
                    pgn.header().add("Result", "1-0");
                    break;
                case Result::WhiteWin:
                    scores_.at(game.name1).losses++;
                    scores_.at(game.name2).wins++;
                    pgn.header().add("Result", "0-1");
                    break;
                case Result::Draw:
                    scores_.at(game.name1).draws++;
                    scores_.at(game.name2).draws++;
                    pgn.header().add("Result", "1/2-1/2");
                    break;
                case Result::None:
                    break;
                default:
                    break;
            }

            // Open pgn file
            pgn_file_.open("games.pgn", std::fstream::out | std::fstream::app);

            // Append game .pgn
            if (pgn_file_.is_open()) {
                pgn_file_ << pgn;
            }

            // Close pgn file
            pgn_file_.close();

            // Only print individual game results if in verbose mode
            if (settings_.verbose) {
                std::cout << "[" << games_completed_ << "/"
                          << settings_.num_games << "] ";

                std::cout << game.name1 << " vs " << game.name2 << " ";

                switch (pos.result()) {
                    case Result::BlackWin:
                        std::cout << "1-0";
                        break;
                    case Result::WhiteWin:
                        std::cout << "0-1";
                        break;
                    case Result::Draw:
                        std::cout << "1/2-1/2";
                        break;
                    case Result::None:
                        std::cout << "*";
                        break;
                    default:
                        std::cout << "?";
                        break;
                }

                std::cout << std::endl;
                std::cout << std::endl;
            }

            // Print overall results every so often
            if (games_completed_ % settings_.ratinginterval == 0) {
                for (const auto &[name, score] : scores_) {
                    std::cout << name << ": " << score << std::endl;
                }

                std::cout << std::endl;
            }
        }
    }
}
