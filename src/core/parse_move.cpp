#include "parse_move.hpp"

[[nodiscard]] auto parse_move(std::string str) -> libataxx::Move {
    // lowercase
    for (auto &c : str) {
        c = tolower(c);
    }

    // Passing moves might be encoded several ways, including [sq][sq]
    if (str == "0000" || str == "pass" || str == "null" || (str.size() == 4 && str[0] == str[2] && str[1] == str[3])) {
        return libataxx::Move::nullmove();
    }

    // Single moves
    if (str.length() == 2 || (str.length() == 4 && str[1] == '@')) {
        const int x = str[str.size() - 2] - 'a';
        const int y = str[str.size() - 1] - '1';

        if (x < 0 || x > 6 || y < 0 || y > 6) {
            throw std::invalid_argument("Not a move. (" + str.substr(2, 2) + ")");
        }

        return libataxx::Move(libataxx::Square{x, y});
    }

    // Double moves
    if (str.length() == 4) {
        const int x1 = str[0] - 'a';
        const int y1 = str[1] - '1';
        const int x2 = str[2] - 'a';
        const int y2 = str[3] - '1';

        if (x1 < 0 || x1 > 6 || y1 < 0 || y1 > 6) {
            throw std::invalid_argument("Invalid move. (" + str + ")");
        }

        if (x2 < 0 || x2 > 6 || y2 < 0 || y2 > 6) {
            throw std::invalid_argument("Invalid move. (" + str + ")");
        }

        const auto sq1 = libataxx::Square{x1, y1};
        const auto sq2 = libataxx::Square{x2, y2};
        const int dx = std::abs(x1 - x2);
        const int dy = std::abs(y1 - y2);

        if (sq1 == sq2) {
            throw std::invalid_argument("Source and destination square are the same");
        }

        if (dx > 2 || dy > 2) {
            throw std::invalid_argument("Invalid move. (" + str + ")");
        }

        // We were just given a single move in longhand notation
        // e.g. "b2b3" instead of "b3"
        if (dx <= 1 && dy <= 1) {
            return libataxx::Move(sq2);
        }

        return libataxx::Move(sq1, sq2);
    }

    throw std::invalid_argument("Invalid move. (" + str + ")");
}
