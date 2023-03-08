#ifndef PGN_HPP
#define PGN_HPP

#include <string>
#include "play.hpp"

struct PGNSettings {
    std::string path = "games.pgn";
    std::string event = "*";
    std::string colour1 = "black";
    std::string colour2 = "white";
    bool enabled = true;
    bool verbose = false;
    bool override = false;
};

auto write_as_pgn(const PGNSettings &settings,
                  const std::string &player1,
                  const std::string &player2,
                  const GameThingy &data) -> void;

#endif
