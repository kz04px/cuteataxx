#include "run.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <queue>
#include <thread>
#include <vector>
#include "results.hpp"
#include "settings.hpp"
#include "worker.hpp"
// Tournaments
#include "tournament/gauntlet.hpp"
#include "tournament/generator.hpp"
#include "tournament/roundrobin.hpp"

void run(const Settings &settings, const std::vector<std::string> &openings) {
    // Create results & initialise
    Results results;
    for (const auto &engine : settings.engines) {
        results.scores[engine.name];
    }

    // Create tournament
    std::shared_ptr<TournamentGenerator> game_generator;

    if (settings.tournament_type == TournamentType::RoundRobin) {
        game_generator =
            std::make_shared<RoundRobinGenerator>(settings.engines.size(), settings.num_games, openings.size(), true);
    } else if (settings.tournament_type == TournamentType::Gauntlet) {
        game_generator =
            std::make_shared<GauntletGenerator>(settings.engines.size(), settings.num_games, openings.size(), true);
    } else {
        game_generator =
            std::make_shared<RoundRobinGenerator>(settings.engines.size(), settings.num_games, openings.size(), true);
    }

    // Create threads
    std::vector<std::thread> threads;

    // Start timer
    const auto t0 = std::chrono::high_resolution_clock::now();

    // Start game threads
    for (int i = 0; i < settings.concurrency; ++i) {
        threads.emplace_back(worker, settings, openings, game_generator, std::ref(results));
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
}
