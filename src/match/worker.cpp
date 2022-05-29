#include "worker.hpp"
#include <elo.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <libataxx/pgn.hpp>
#include <mutex>
#include "play.hpp"
#include "results.hpp"
#include "settings.hpp"

std::mutex mtx_output;
std::mutex mtx_games;

void print_score(const EngineSettings &engine1,
                 const EngineSettings &engine2,
                 const Results &results,
                 const bool show_elo = true) {
    const auto w = results.scores.at(engine1.name).wins;
    const auto l = results.scores.at(engine1.name).losses;
    const auto d = results.scores.at(engine1.name).draws;

    std::cout << "Score of ";
    std::cout << engine1.name << " vs " << engine2.name;
    std::cout << ": " << results.scores.at(engine1.name);
    std::cout << " " << results.scores.at(engine1.name).played;
    std::cout << "\n";
    if (show_elo) {
        std::cout << std::fixed << std::setprecision(2) << get_elo(w, l, d) << " +/- " << get_err(w, l, d) << "\n";
    }
}

void worker(const Settings &settings, std::stack<GameSettings> &games, Results &results) {
    while (true) {
        GameSettings game;

        // Get something to do
        {
            std::lock_guard<std::mutex> lock(mtx_games);
            if (games.empty()) {
                return;
            }
            game = games.top();
            games.pop();
        }

        if (settings.verbose) {
            std::lock_guard<std::mutex> lock(mtx_output);
            std::cout << "Starting game " << game.engine1.name << " vs " << game.engine2.name << "\n";
        }

        libataxx::pgn::PGN pgn;

        // Play the game
        try {
            pgn = play(settings, game);
        } catch (std::invalid_argument &e) {
            std::cerr << e.what() << "\n";
        } catch (const char *e) {
            std::cerr << e << "\n";
        } catch (std::exception &e) {
            std::cerr << e.what() << "\n";
        } catch (...) {
            std::cerr << "Error woops\n";
            continue;
        }

        // Results & printing
        {
            std::lock_guard<std::mutex> lock(mtx_output);

            if (settings.verbose) {
                std::cout << "Finishing game " << game.engine1.name << " vs " << game.engine2.name << "\n";
            }

            results.games_played++;

            // Update engine results
            results.scores[game.engine1.name].played++;
            results.scores[game.engine2.name].played++;
            if (pgn.header().get("Result") == "1-0") {
                results.scores[game.engine1.name].wins++;
                results.scores[game.engine2.name].losses++;
                results.black_wins++;
            } else if (pgn.header().get("Result") == "0-1") {
                results.scores[game.engine1.name].losses++;
                results.scores[game.engine2.name].wins++;
                results.white_wins++;
            } else if (pgn.header().get("Result") == "1/2-1/2") {
                results.scores[game.engine1.name].draws++;
                results.scores[game.engine2.name].draws++;
                results.draws++;
            }

            // Write to .pgn
            if (settings.pgn_enabled && !settings.pgn_path.empty()) {
                std::ofstream file(settings.pgn_path, std::fstream::out | std::fstream::app);

                if (file.is_open()) {
                    file << pgn;
                } else {
                    std::cerr << "Could not open " << settings.pgn_path << "\n";
                }
            }

            // Print results
            if (results.scores.size() == 2) {
                if (results.games_played < settings.ratinginterval ||
                    results.games_played % settings.ratinginterval == 0 ||
                    results.games_played == results.games_total) {
                    const bool show_elo = results.games_played >= settings.ratinginterval;

                    if (results.games_played > settings.ratinginterval) {
                        std::cout << "\n";
                    }

                    if (game.engine1.id < game.engine2.id) {
                        print_score(game.engine1, game.engine2, results, show_elo);
                    } else {
                        print_score(game.engine2, game.engine1, results, show_elo);
                    }
                }
            } else if (results.games_played % settings.ratinginterval == 0 ||
                       results.games_played == results.games_total) {
                for (const auto &[name, score] : results.scores) {
                    std::cout << name << ": ";
                    std::cout << score;
                    std::cout << " " << score.played;
                    std::cout << "\n";
                }
                std::cout << "\n";
            }
        }
    }
}
