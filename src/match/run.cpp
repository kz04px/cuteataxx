#include <chrono>
#include <iostream>
#include <thread>
#include "game.hpp"
#include "match.hpp"
#include "results.hpp"
#include "settings.hpp"

using namespace std::chrono;

namespace match {

void Match::run(const Settings &settings,
                const Openings &openings,
                const Engines &engines) {
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
    int idx_opening = 0;
    for (int i = 0; i < settings.num_games; i += 2) {
        for (const auto &[name1, engine1] : engines) {
            for (const auto &[name2, engine2] : engines) {
                // Engines can't play themselves
                if (name1 == name2) {
                    continue;
                }

                games.push(Game{openings[idx_opening], engine1, engine2});
            }
        }

        // Next opening
        idx_opening++;
        idx_opening = idx_opening % openings.size();
    }

    // Create results & initialise
    Results results;
    for (const auto &[key, val] : engines) {
        results.scores[key];
    }

    // Create threads
    std::thread threads[settings.concurrency];

    // Start timer
    const auto t0 = high_resolution_clock::now();

    // Start game threads
    for (int i = 0; i < settings.concurrency; ++i) {
        threads[i] = std::thread(
            &Match::worker, this, settings, std::ref(games), std::ref(results));
    }

    // Wait for game threads to finish
    for (int i = 0; i < settings.concurrency; ++i) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }

    // End timer
    const auto t1 = high_resolution_clock::now();

    // Elapsed time
    const auto diff = duration_cast<seconds>(t1 - t0);

    // Print stats
    std::cout << "Time taken: " << diff.count() << std::endl;
    std::cout << "Total games: " << settings.num_games << std::endl;
    std::cout << "Threads: " << settings.concurrency << std::endl;
    std::cout << std::endl;

    // Print engine scores
    for (const auto &[name, score] : results.scores) {
        std::cout << "Score of " << name << ": ";
        std::cout << score;
        std::cout << " " << score.played;
        std::cout << std::endl;
    }
}

}  // namespace match
