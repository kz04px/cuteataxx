#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>
#include "game.hpp"
#include "match.hpp"
#include "results.hpp"
#include "settings.hpp"

void Match::run(const Settings &settings, const Openings &openings, const Engines &engines) {
    if (openings.size() < 1) {
        throw std::invalid_argument("Must be at least 1 opening position");
    }

    if (engines.size() < 2) {
        throw std::invalid_argument("Must be at least 2 engines");
    }

    if (settings.concurrency < 1) {
        throw std::invalid_argument("Must be at least 1 thread");
    }

    // Create games
    std::stack<Game> games;
    for (std::size_t i = 0; i < engines.size(); ++i) {
        for (std::size_t j = i + 1; j < engines.size(); ++j) {
            int idx_opening = 0;

            for (int n = 0; n < settings.num_games; n += 2) {
                games.push(Game{openings[idx_opening], engines[i], engines[j]});
                games.push(Game{openings[idx_opening], engines[j], engines[i]});

                // Next opening
                idx_opening++;
                idx_opening = idx_opening % openings.size();
            }
        }
    }

    // Create results & initialise
    Results results;
    for (const auto &engine : engines) {
        results.scores[engine.name];
    }

    // Create threads
    std::vector<std::thread> threads;

    // Start timer
    const auto t0 = std::chrono::high_resolution_clock::now();

    // Start game threads
    for (int i = 0; i < settings.concurrency; ++i) {
        threads.emplace_back(&Match::worker, this, settings, std::ref(games), std::ref(results));
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
    std::cout << "Total games: " << settings.num_games << "\n";
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
