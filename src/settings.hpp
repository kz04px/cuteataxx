#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <libataxx/search.hpp>
#include <string>

struct Settings {
    int games = 1;
    int concurrency = 1;
    int ratinginterval = 1;
    bool recover = false;
    bool verbose = false;
    bool repeat = false;
    bool debug = false;
    bool pgnout = false;
    std::string openings_path;
    std::string pgn_path;
    libataxx::SearchOptions search;
};

#endif
