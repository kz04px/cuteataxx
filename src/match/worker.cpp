#include <fstream>
#include <iomanip>
#include <iostream>
#include "../elo.hpp"
#include "match.hpp"
#include "settings.hpp"

void print_score(const match::Details &engine1,
                 const match::Details &engine2,
                 const match::Results &results,
                 const bool show_elo = true) {
    const auto e = Elo{results.scores.at(engine1.name).wins,
                       results.scores.at(engine1.name).losses,
                       results.scores.at(engine1.name).draws};

    std::cout << "Score of ";
    std::cout << engine1.name << " vs " << engine2.name;
    std::cout << ": " << results.scores.at(engine1.name);
    std::cout << " " << results.scores.at(engine1.name).played;
    std::cout << std::endl;
    if (show_elo) {
        std::cout << std::fixed << std::setprecision(2) << e.elo() << " +/- "
                  << e.err();
    }
}

namespace match {

void Match::worker(const Settings &settings,
                   std::stack<Game> &games,
                   Results &results) {
    while (true) {
        Game game;

        // Get something to do
        {
            std::lock_guard<std::mutex> lock(mtx_games_);
            if (games.empty()) {
                return;
            }
            game = games.top();
            games.pop();
            if (settings.verbose) {
                std::cout << "Starting game " << game.engine1.name << " vs "
                          << game.engine2.name << std::endl;
            }
        }

        libataxx::pgn::PGN pgn;

        // Play the game
        try {
            pgn = play(settings, game);
        } catch (std::invalid_argument &e) {
            std::cerr << e.what() << std::endl;
        } catch (const char *e) {
            std::cerr << e << std::endl;
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Error woops" << std::endl;
            continue;
        }

        // Results & printing
        {
            std::lock_guard<std::mutex> lock(mtx_output_);

            results.games_played++;

            // Update engine results
            results.scores[game.engine1.name].played++;
            results.scores[game.engine2.name].played++;
            if (pgn.header().get("Result") == "1-0") {
                results.scores[game.engine1.name].wins++;
                results.scores[game.engine2.name].losses++;
            } else if (pgn.header().get("Result") == "0-1") {
                results.scores[game.engine1.name].losses++;
                results.scores[game.engine2.name].wins++;
            } else if (pgn.header().get("Result") == "1/2-1/2") {
                results.scores[game.engine1.name].draws++;
                results.scores[game.engine2.name].draws++;
            }

            // Write to .pgn
            if (settings.pgn_enabled && !settings.pgn_path.empty()) {
                std::ofstream file(settings.pgn_path,
                                   std::fstream::out | std::fstream::app);

                if (file.is_open()) {
                    file << pgn;
                } else {
                    std::cerr << "Could not open " << settings.pgn_path
                              << std::endl;
                }
            }

            // Print results
            if (results.scores.size() == 2) {
                if (results.games_played < settings.ratinginterval ||
                    results.games_played % settings.ratinginterval == 0) {
                    const bool show_elo =
                        results.games_played >= settings.ratinginterval;
                    if (game.engine1.id < game.engine2.id) {
                        print_score(
                            game.engine1, game.engine2, results, show_elo);
                    } else {
                        print_score(
                            game.engine2, game.engine1, results, show_elo);
                    }

                    if (results.games_played >= settings.ratinginterval) {
                        std::cout << std::endl;
                    }
                }
            } else {
                if (results.games_played % settings.ratinginterval == 0) {
                    for (const auto &[name, score] : results.scores) {
                        std::cout << name << ": ";
                        std::cout << score;
                        std::cout << " " << score.played;
                        std::cout << std::endl;
                    }
                    std::cout << std::endl;
                }
            }
        }
    }
}  // namespace match

}  // namespace match
