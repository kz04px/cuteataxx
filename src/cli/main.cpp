#include <chrono>
#include <elo.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sprt.hpp>
#include <stdexcept>
#include <thread>
#include "core/engine/engine.hpp"
#include "core/match/callbacks.hpp"
#include "core/match/run.hpp"
#include "core/match/settings.hpp"
#include "core/parse/openings.hpp"
#include "core/parse/settings.hpp"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Must provide path to settings file\n";
        return 1;
    }

    try {
        const auto settings = parse::settings(argv[1]);
        const auto openings = parse::openings(settings.openings_path, settings.shuffle);
        const auto callbacks = Callbacks{
            .on_engine_start =
                [&settings](const std::string &name) {
                    if (settings.verbose) {
                        std::cout << "Created engine " << name << std::endl;
                    }
                },
            .on_game_started =
                [&settings](const int game_id, const std::string &engine1, const std::string &engine2) {
                    if (settings.verbose) {
                        std::cout << "Started game " << engine1 << " vs " << engine2 << std::endl;
                    }
                },
            .on_game_finished =
                [&settings](const int game_id, const std::string &engine1, const std::string &engine2) {
                    if (settings.verbose) {
                        std::cout << "Finished game " << engine1 << " vs " << engine2 << std::endl;
                    }
                },
            .on_results_update =
                [&settings](const Results &results) {
                    if (settings.engines.size() == 2) {
                        const auto &e1 = settings.engines.at(0);
                        const auto &e2 = settings.engines.at(1);
                        const auto w = results.scores.at(e1.name).wins;
                        const auto l = results.scores.at(e1.name).losses;
                        const auto d = results.scores.at(e1.name).draws;
                        const auto elo = get_elo(w, l, d);
                        const auto err = get_err(w, l, d);
                        const auto llr = sprt::get_llr(w, l, d, settings.sprt.elo0, settings.sprt.elo1);
                        const auto lbound = sprt::get_lbound(settings.sprt.alpha, settings.sprt.beta);
                        const auto ubound = sprt::get_ubound(settings.sprt.alpha, settings.sprt.beta);

                        const auto is_sprt_stop = settings.sprt.enabled && (llr <= lbound || llr >= ubound);
                        const auto is_print_early =
                            results.games_played < settings.ratinginterval && settings.print_early;
                        const auto is_print_late = results.games_played % settings.ratinginterval == 0;
                        const auto is_complete = settings.num_games == results.games_played;

                        const auto print_result = is_print_early || is_print_late || is_sprt_stop || is_complete;
                        const auto print_elo =
                            results.games_played >= settings.ratinginterval || is_sprt_stop || is_complete;
                        const auto print_sprt = settings.sprt.enabled && print_elo;

                        if (!print_result) {
                            return;
                        }

                        const auto point_percentage = (2.0 * w + d) / (2.0 * results.games_played);

                        // Print score
                        std::cout << "Score of ";
                        std::cout << e1.name << " vs " << e2.name;
                        std::cout << ": " << w << " - " << l << " - " << d;
                        std::cout << "  [" << std::fixed << std::setprecision(3) << point_percentage << "]";
                        std::cout << " " << results.games_played;
                        std::cout << std::endl;

                        // Print Elo
                        if (print_elo) {
                            std::cout << "Elo difference: ";
                            std::cout << std::fixed << std::setprecision(2) << elo << " +/- " << err;
                            std::cout << ", LOS: " << los(w, l) << " %";
                            std::cout << ", DrawRatio: " << ((100.0 * d) / results.games_played) << " %";
                            std::cout << std::endl;
                        }

                        // Print SPRT
                        if (print_sprt) {
                            std::cout << "SPRT: llr " << llr << ", lbound " << lbound << ", ubound " << ubound << "\n";
                        }

                        // Spacer
                        if (print_elo || print_sprt) {
                            std::cout << std::endl;
                        }
                    } else {
                        const auto is_print_late = results.games_played % settings.ratinginterval == 0;
                        const auto is_complete = settings.num_games == results.games_played;
                        const auto print_result = is_print_late || is_complete;

                        if (!print_result) {
                            return;
                        }

                        auto name_length = 8;
                        auto max_wins = 999;
                        auto max_losses = 9999;
                        auto max_draws = 9999;
                        auto max_played = 999999;

                        for (const auto &[name, score] : results.scores) {
                            if (name.size() > name_length) {
                                name_length = name.size() + 2;
                            }

                            if (score.wins > max_wins) {
                                max_wins = score.wins;
                            }

                            if (score.losses > max_losses) {
                                max_losses = score.losses;
                            }

                            if (score.draws > max_draws) {
                                max_draws = score.draws;
                            }

                            if (score.draws > max_draws) {
                                max_draws = score.draws;
                            }
                        }

                        const auto win_length = std::to_string(max_wins).size() + 2;
                        const auto lose_length = std::to_string(max_losses).size() + 2;
                        const auto draw_length = std::to_string(max_draws).size() + 2;
                        const auto played_length = std::to_string(max_played).size() + 2;

                        std::cout << std::setw(name_length) << std::left << "Engines";
                        std::cout << std::setw(win_length) << std::right << "Win";
                        std::cout << std::setw(lose_length) << std::right << "Lose";
                        std::cout << std::setw(draw_length) << std::right << "Draw";
                        std::cout << std::setw(played_length) << std::right << "Played";
                        std::cout << std::setw(7) << std::right << "Rate";
                        std::cout << "\n";
                        for (const auto &[name, score] : results.scores) {
                            const float points = score.wins + static_cast<float>(score.draws) / 2;
                            const float rate = score.played ? points / score.played : 0.0f;

                            std::cout << std::setw(name_length) << std::left << name;
                            std::cout << std::setw(win_length) << std::right << score.wins;
                            std::cout << std::setw(lose_length) << std::right << score.losses;
                            std::cout << std::setw(draw_length) << std::right << score.draws;
                            std::cout << std::setw(played_length) << std::right << score.played;
                            std::cout << std::setw(7) << std::right << std::fixed << std::setprecision(3) << rate;
                            std::cout << "\n";
                        }
                        std::cout << "\n";
                    }
                },
            .on_info_send =
                [](const std::string &msg) {
                    std::cout << std::this_thread::get_id() << "> " << msg << "\n";
                },
            .on_info_recv =
                [](const std::string &msg) {
                    std::cout << std::this_thread::get_id() << "< " << msg << "\n";
                },
        };

        // Clear pgn
        if (settings.pgn.override) {
            std::ofstream file(settings.pgn.path, std::ofstream::trunc);
        }

        std::cout << "Settings:\n";
        std::cout << "- games " << settings.num_games << "\n";
        std::cout << "- engines " << settings.engines.size() << "\n";
        std::cout << "- concurrency " << settings.concurrency << "\n";
        std::cout << "- timecontrol " << settings.tc << "\n";
        std::cout << "- openings " << openings.size() << "\n";
        std::cout << "\n";

        // Start timer
        const auto t0 = std::chrono::high_resolution_clock::now();

        const auto results = run(settings, openings, callbacks);

        // End timer
        const auto t1 = std::chrono::high_resolution_clock::now();
        const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
        const auto hh_mm_ss = std::chrono::hh_mm_ss(diff);

        // Print stats
        std::cout << "\n";
        std::cout << "Time taken: ";
        std::cout << std::setfill('0') << std::setw(2) << hh_mm_ss.hours().count() << "h ";
        std::cout << std::setfill('0') << std::setw(2) << hh_mm_ss.minutes().count() << "m ";
        std::cout << std::setfill('0') << std::setw(2) << hh_mm_ss.seconds().count() << "s\n";
        std::cout << "Total games: " << results.games_played << "\n";
        std::cout << "Threads: " << settings.concurrency << "\n";
        if (diff.count() > 0) {
            const auto games_per_ms = static_cast<float>(results.games_played) / diff.count();
            const auto games_per_sec = games_per_ms * 1000;
            std::cout << std::setprecision(games_per_sec >= 100 ? 0 : 2);
            std::cout << "games/sec: " << games_per_sec << "\n";
            std::cout << "games/min: " << games_per_sec * 60.0f << "\n";
        }
        std::cout << "\n";

        // Print match statistics
        std::cout << "Result  Games\n";
        std::cout << "1-0     " << results.black_wins << "\n";
        std::cout << "0-1     " << results.white_wins << "\n";
        std::cout << "1/2-1/2 " << results.draws << "\n";
    } catch (std::invalid_argument &e) {
        std::cerr << e.what() << "\n";
    } catch (const char *e) {
        std::cerr << e << "\n";
    } catch (std::exception &e) {
        std::cerr << e.what() << "\n";
    } catch (...) {
        std::cerr << "Uh oh\n";
    }

    return 0;
}
