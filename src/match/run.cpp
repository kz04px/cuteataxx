#include "run.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <stack>
#include <thread>
#include <vector>
#include "results.hpp"
#include "settings.hpp"
#include "worker.hpp"

void run(const Settings &settings, const std::vector<std::string> &openings) {
    // Create games
    std::stack<GameSettings> games;
    for (std::size_t i = 0; i < settings.engines.size(); ++i) {
        for (std::size_t j = i + 1; j < settings.engines.size(); ++j) {
            int idx_opening = 0;

            for (int n = 0; n < settings.num_games; n += 2) {
                games.push(GameSettings{openings[idx_opening], settings.engines[i], settings.engines[j]});
                games.push(GameSettings{openings[idx_opening], settings.engines[j], settings.engines[i]});

                // Next opening
                idx_opening++;
                idx_opening = idx_opening % openings.size();
            }
        }
    }

    const auto num_games = games.size();

    // Create results & initialise
    Results results;
    for (const auto &engine : settings.engines) {
        results.scores[engine.name];
    }

    // Create threads
    std::vector<std::thread> threads;

    // Start timer
    const auto t0 = std::chrono::high_resolution_clock::now();

    // Start game threads
    for (int i = 0; i < settings.concurrency; ++i) {
        threads.emplace_back(worker, settings, std::ref(games), std::ref(results));
    }

    // Wait for game threads to finish
    for (auto &thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    // End timer
    const auto t1 = std::chrono::high_resolution_clock::now();

    // Elapsed time
    const auto diff = std::chrono::duration_cast<std::chrono::seconds>(t1 - t0);
    const auto hh_mm_ss = std::chrono::hh_mm_ss(diff);

    // Print stats
    std::cout << "\n\n";
    std::cout << "Time taken: ";
    std::cout << std::setfill('0') << std::setw(2) << hh_mm_ss.hours().count() << "h ";
    std::cout << std::setfill('0') << std::setw(2) << hh_mm_ss.minutes().count() << "m ";
    std::cout << std::setfill('0') << std::setw(2) << hh_mm_ss.seconds().count() << "s\n";
    std::cout << "Total games: " << num_games << "\n";
    std::cout << "Threads: " << settings.concurrency << "\n";
    std::cout << "\n";

    // Print engine scores
    for (const auto &[name, score] : results.scores) {
        std::cout << "Score of " << name << ": ";
        std::cout << score;
        std::cout << " " << score.played;
        std::cout << "\n";
    }
}
