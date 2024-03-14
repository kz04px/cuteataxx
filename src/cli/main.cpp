#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include "core/engine/engine.hpp"
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

        std::cout << "Settings:\n";
        std::cout << "- games " << settings.num_games << "\n";
        std::cout << "- engines " << settings.engines.size() << "\n";
        std::cout << "- concurrency " << settings.concurrency << "\n";
        std::cout << "- timecontrol " << settings.tc << "\n";
        std::cout << "- openings " << openings.size() << "\n";
        std::cout << "\n";

        // Clear pgn
        if (settings.pgn.override) {
            std::ofstream file(settings.pgn.path, std::ofstream::trunc);
        }

        // Start timer
        const auto t0 = std::chrono::high_resolution_clock::now();

        const auto results = run(settings, openings);

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
