#include <CLI11.hpp>
#include <cassert>
#include <iostream>
#include <limits>
#include <string>
#include "match.hpp"
#include "settings.hpp"

int main(int argc, char **argv) {
    CLI::App app{"Cuteataxx"};

    Settings settings;
    std::vector<Player> players;
    std::vector<std::string> openings;

    // Specify command line options
    app.add_option(
           "--openings", settings.openings_path, "Path to openings file")
        ->check(CLI::ExistingFile);
    app.add_option("--pgn", settings.pgn_path, "Path to pgn output");
    app.add_option("--games", settings.games, "Number of games to play")
        ->check(CLI::Range(1, 100000))
        ->required();
    app.add_option("--concurrency", settings.concurrency, "Threads to use")
        ->check(CLI::Range(1, 128));
    app.add_option(
           "--ratinginterval", settings.ratinginterval, "Rating interval")
        ->check(CLI::Range(1, 10000));

    // Specify command line options -- Search type
    app.add_option_function<std::string>(
        "--tc",
        [&settings](const std::string &val) {
            settings.search.type = libataxx::engine::SearchSettings::Type::Time;
            // Fixme
            settings.search.btime = 1000;
            settings.search.wtime = 1000;
            settings.search.binc = 10;
            settings.search.winc = 10;
        },
        "Search time control");
    app.add_option_function<std::uint64_t>(
           "--nodes",
           [&settings](const int &val) {
               settings.search.type =
                   libataxx::engine::SearchSettings::Type::Nodes;
               settings.search.nodes = val;
           },
           "Search nodes")
        ->check(CLI::Range(static_cast<std::uint64_t>(1),
                           std::numeric_limits<std::uint64_t>::max()));
    app.add_option_function<int>(
           "--depth",
           [&settings](const int &val) {
               settings.search.type =
                   libataxx::engine::SearchSettings::Type::Depth;
               settings.search.depth = val;
           },
           "Search depth")
        ->check(CLI::Range(1, std::numeric_limits<int>::max()));
    app.add_option_function<int>(
           "--movetime",
           [&settings](const int &val) {
               settings.search.type =
                   libataxx::engine::SearchSettings::Type::Movetime;
               settings.search.movetime = val;
           },
           "Search movetime")
        ->check(CLI::Range(1, std::numeric_limits<int>::max()));

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

    // Get players
    players.push_back(Player{.name = "Tiktaxx 1", .path = "./engines/tiktaxx"});
    players.push_back(Player{.name = "Tiktaxx 2", .path = "./engines/tiktaxx"});

    // Get openings
    if (settings.openings_path.empty()) {
        openings.push_back("x5o/7/7/7/7/7/o5x x 0");
    } else {
        std::ifstream file(settings.openings_path);

        if (!file.is_open()) {
            std::cerr << "Could not open " << settings.openings_path
                      << std::endl;
            return 1;
        }

        std::string line;
        while (getline(file, line)) {
            openings.push_back(line);
        }

        if (openings.size() == 0) {
            std::cerr << "No legal positions found in "
                      << settings.openings_path << std::endl;
            return 2;
        }
    }

    // Play match
    match(settings, players, openings);

    return 0;
}
