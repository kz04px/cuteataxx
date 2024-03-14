#include "openings.hpp"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <random>
#include <stdexcept>
#include "../match/settings.hpp"

namespace parse {

[[nodiscard]] std::vector<std::string> openings(const std::string &path, const bool shuffle) {
    std::ifstream f(path);
    std::vector<std::string> openings;

    if (!f.is_open()) {
        openings.push_back("x5o/7/7/7/7/7/o5x x 0 1");
        return openings;
    }

    const auto min_fen_size = std::string("7/7/7/7/7/7/7").size();

    std::string line;
    while (std::getline(f, line)) {
        if (line.size() < min_fen_size || line[0] == '#') {
            continue;
        }
        openings.push_back(line);
    }

    if (openings.empty()) {
        throw std::invalid_argument("Must be at least 1 opening position");
    }

    if (shuffle) {
        std::mt19937 rng(std::time(nullptr));
        std::shuffle(openings.begin(), openings.end(), rng);
    }

    return openings;
}

}  // namespace parse
