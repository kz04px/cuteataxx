#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>

struct Player {
    std::string path;
    int time = 10000;
    int inc = 1000;
};

#endif
