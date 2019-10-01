#include <chrono>
#include <iostream>
#include "match.hpp"

using namespace std::chrono;

void Match::run() {
    if (settings_.openings.size() < 1) {
        throw std::invalid_argument("Must be at least 1 opening position");
    }

    if (settings_.engines.size() < 2) {
        throw std::invalid_argument("Must be at least 2 engines");
    }

    std::thread threads[settings_.concurrency];

    // Initialise engine scores
    for (const auto& [name, settings] : settings_.engines) {
        scores_[name];
    }

    // Create games
    int idx_opening = 0;
    for (int i = 0; i < settings_.num_games; i += 2) {
        for (const auto& [name1, engine1] : settings_.engines) {
            for (const auto& [name2, engine2] : settings_.engines) {
                // Engines can't play themselves
                if (name1 == name2) {
                    continue;
                }

                games_.push(
                    Game{settings_.openings[idx_opening], name1, name2});
            }
        }

        // Next opening
        idx_opening++;
        idx_opening = idx_opening % settings_.openings.size();
    }

    // Start timer
    const auto t0 = high_resolution_clock::now();

    // Start game threads
    for (int i = 0; i < settings_.concurrency; ++i) {
        threads[i] = std::thread(&Match::play, this);
    }

    // Wait for game threads to finish
    for (int i = 0; i < settings_.concurrency; ++i) {
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
    std::cout << "Total games: " << settings_.num_games << std::endl;
    std::cout << "Threads: " << settings_.concurrency << std::endl;

    // Print engine scores
    for (const auto& [name, score] : scores_) {
        std::cout << name << ": " << score << std::endl;
    }
}
