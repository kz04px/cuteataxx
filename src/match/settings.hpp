#ifndef MATCH_SETTINGS_HPP
#define MATCH_SETTINGS_HPP

#include <limits>
#include <map>
#include <string>
#include <vector>
#include "../uaiengine.hpp"

namespace match {

struct Settings {
    int ratinginterval = 10;
    int concurrency = 1;
    int num_games = 100;
    int hash_size = 16;
    int timeout_buffer = 10;
    unsigned int maxfullmoves = std::numeric_limits<unsigned int>::max();
    bool debug = false;
    bool recover = false;
    bool verbose = false;
    bool repeat = true;
    SearchSettings tc;
    std::string colour1 = "white";
    std::string colour2 = "black";
    std::string openings_path = "";
    // pgn
    bool pgn_enabled = true;
    bool pgn_verbose = false;
    bool pgn_override = false;
    std::string pgn_path = "games.pgn";
    std::string pgn_event = "*";
};

}  // namespace match

#endif
