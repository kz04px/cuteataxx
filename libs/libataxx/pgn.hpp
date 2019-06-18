#ifndef LIBATAXX_PGN_HPP
#define LIBATAXX_PGN_HPP

#include <fstream>
#include <string>
#include <unordered_map>
#include "position.hpp"

namespace libataxx {

namespace pgn {

struct Game {
    Game() : header{}, position{} {
    }

    static Game from_position(Position &pos) {
        return Game{};
    }

    std::unordered_map<std::string, std::string> header;
    Position position;
};

std::ofstream &operator<<(std::ofstream &ofs, const Game &game) {
    // Header -- Event
    auto asd = game.header.find("Event");
    if (asd == game.header.end()) {
        ofs << "[Event \"None\"]" << std::endl;
    } else {
        ofs << "[Event \"" << asd->second << "\"]" << std::endl;
    }

    // Header -- The rest
    for (const auto &[key, val] : game.header) {
        if (key == "Event") {
            continue;
        }
        ofs << "[" << key << " \"" << val << "\"]" << std::endl;
    }
    ofs << std::endl << std::endl;

    // Moves
    int movenum = 1;
    for (const auto &move : game.position.history()) {
        ofs << " " << move;
    }
    ofs << std::endl << std::endl;

    return ofs;
}

}  // namespace pgn

}  // namespace libataxx

#endif
