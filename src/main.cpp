#include <iostream>
#include <libataxx/test.hpp>
#include <string>
#include "match.hpp"
#include "settings.hpp"

int main(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-debug") {
            settings::debug = true;
        } else if (std::string(argv[i]) == "-recover") {
            settings::repeat = true;
        } else if (std::string(argv[i]) == "-repeat") {
            settings::repeat = true;
        } else if (std::string(argv[i]) == "-verbose") {
            settings::verbose = true;
        } else {
            std::cerr << "Unknown parameter: " << argv[i] << std::endl;
        }
    }

    libataxx::test();

    std::vector<Player> players;
    players.push_back(Player{.path = "engines/tiktaxx"});
    players.push_back(Player{.path = "engines/skeleton-debug"});

    std::vector<std::string> openings;
    openings.push_back("x5o/7/7/7/7/7/o5x x");
    openings.push_back("x5o/7/2-1-2/7/2-1-2/7/o5x x");

    match(1, 10, players, openings);

    return 0;
}
