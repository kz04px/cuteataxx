#include "openings.hpp"
#include <fstream>
#include "../match/settings.hpp"

namespace parse {

[[nodiscard]] std::vector<std::string> openings(const std::string &path) {
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

    return openings;
}

}  // namespace parse
