#ifndef MATCH_GAME_HPP
#define MATCH_GAME_HPP

#include <string>
#include "engines.hpp"

namespace match {

struct Game {
    std::string fen;
    Details engine1;
    Details engine2;
};

}  // namespace match

#endif
