#ifndef MATCH_SETTINGS_HPP
#define MATCH_SETTINGS_HPP

#include <libataxx/engine.hpp>
#include <map>
#include <string>
#include <vector>

namespace match {

struct Settings {
    int ratinginterval = 10;
    int concurrency = 1;
    int num_games = 100;
    int hash_size = 16;
    int timeout_buffer = 10;
    bool debug = false;
    bool recover = false;
    bool verbose = false;
    bool repeat = true;
    libataxx::engine::SearchSettings tc;
    std::string colour1 = "white";
    std::string colour2 = "black";
    std::string openings_path = "";
    // pgn
    bool pgn_enabled = true;
    bool pgn_verbose = false;
    std::string pgn_path = "games.pgn";
    std::string pgn_event = "*";
};

}  // namespace match

#endif
