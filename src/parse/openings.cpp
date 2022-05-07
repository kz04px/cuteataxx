#include "openings.hpp"
#include <fstream>
#include "../match/settings.hpp"

namespace parse {

Openings openings(const std::string &path) {
    std::ifstream f(path);
    Openings openings;

    if (!f.is_open()) {
        openings.push_back("x5o/7/7/7/7/7/o5x x 0");
        return openings;
    }

    std::string line;
    while (std::getline(f, line)) {
        openings.push_back(line);
    }

    return openings;
}

}  // namespace parse
