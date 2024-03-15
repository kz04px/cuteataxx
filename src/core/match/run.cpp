#include "run.hpp"
#include <memory>
#include <thread>
#include <vector>
#include "settings.hpp"
#include "worker.hpp"
// Tournaments
#include "../tournament/gauntlet.hpp"
#include "../tournament/generator.hpp"
#include "../tournament/roundrobin.hpp"

Results run(const Settings &settings, const std::vector<std::string> &openings, const Callbacks &callbacks) {
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

    // Start game threads
    for (int i = 0; i < settings.concurrency; ++i) {
        threads.emplace_back(worker, settings, openings, game_generator, std::ref(results), std::cref(callbacks));
    }

    // Wait for game threads to finish
    for (auto &thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    return results;
}
