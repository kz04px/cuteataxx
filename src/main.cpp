#include <CLI11.hpp>
#include <cassert>
#include <iostream>
#include <limits>
#include <string>
#include "match.hpp"

int main(int argc, char **argv) {
    CLI::App app{"Cuteataxx"};

    MatchSettings settings;
    std::string openings_path;
    std::string pgn_path;

    // Specify command line options
    app.add_option("--openings", openings_path, "Path to openings file")
        ->check(CLI::ExistingFile);
    app.add_option("--pgn", pgn_path, "Path to pgn output");
    app.add_option("--games", settings.num_games, "Number of games to play")
        ->check(CLI::Range(1, 100000))
        ->required();
    app.add_option("--concurrency", settings.concurrency, "Threads to use")
        ->check(CLI::Range(1, 128));
    app.add_option(
           "--ratinginterval", settings.ratinginterval, "Rating interval")
        ->check(CLI::Range(1, 10000));

    // Add engines
    auto engine_option = app.add_option(
        "--engine",
        [&settings](CLI::results_t vals) {
            for (std::size_t i = 0; i < vals.size(); i += 2) {
                settings.add_engine(vals.at(i), vals.at(i + 1));
            }
            return true;
        },
        "Engine name & path");
    engine_option->type_name("name path");
    engine_option->type_size(-2);

    // Specify command line flags
    app.add_flag("--debug ", settings.debug, "Enable debug mode");
    app.add_flag("--recover ", settings.recover, "Enable recover mode");
    app.add_flag("--verbose ", settings.verbose, "Enable verbose mode");

    // Parse command line arguments
    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    // Add openings
    if (openings_path.empty()) {
        settings.add_opening("x5o/7/7/7/7/7/o5x x 0");
    } else {
        std::ifstream file(openings_path);

        if (!file.is_open()) {
            std::cerr << "Could not open " << openings_path << std::endl;
            return 1;
        }

        std::string line;
        while (getline(file, line)) {
            settings.add_opening(line);
        }
    }

    // Create match with the settings
    Match match{settings};

    // Print stats
    match.stats();

    // Play match
    match.run();

    return 0;
}
