#ifndef MATCH_GAME_HPP
#define MATCH_GAME_HPP

#include <string>
#include "engines.hpp"

struct Game {
    std::string fen;
    Details engine1;
    Details engine2;
};

#endif
