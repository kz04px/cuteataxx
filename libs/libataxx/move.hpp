#ifndef LIBATAXX_MOVE_HPP
#define LIBATAXX_MOVE_HPP

#include <iostream>
#include "constants.hpp"
#include "other.hpp"

namespace libataxx {

enum MoveType : bool
{
    Single = 0,
    Double
};

struct Move {
   public:
    explicit Move(const Square to) : to_{to}, from_{to} {
    }
    Move(const Square to, const Square from) : to_{to}, from_{from} {
    }
    [[nodiscard]] Square to() const {
        return to_;
    }
    [[nodiscard]] Square from() const {
        return from_;
    }
    [[nodiscard]] MoveType type() const {
        if (to_ == from_) {
            return MoveType::Single;
        } else {
            return MoveType::Double;
        }
    }
    [[nodiscard]] const std::string san() const {
        return "a1";
        /*
        if (m.type() == MoveType::Single) {
            return static_cast<char>(sq_to_file(from) + 'a') +
                   static_cast<char>(sq_to_rank(from) + '1');
        } else {
            return static_cast<char>(sq_to_file(from) + 'a') +
                   static_cast<char>(sq_to_rank(from) + '1') +
                   static_cast<char>(sq_to_file(to) + 'a') +
                   static_cast<char>(sq_to_rank(to) + '1');
        }*/
    }
    [[nodiscard]] static Move from_san(const std::string &san) {
        if (san.length() == 2) {
            const int x = san[0] - 'a';
            const int y = san[1] - '1';
            const int sq = 7 * y + x;

            if (x < 0 || x > 6 || y < 0 || y > 6) {
                throw std::invalid_argument("Not a move. (" + san + ")");
            }

            return Move(static_cast<Square>(sq));
        } else if (san.length() == 4) {
            const int x1 = san[0] - 'a';
            const int y1 = san[1] - '1';
            const int x2 = san[2] - 'a';
            const int y2 = san[3] - '1';

            if (x1 < 0 || x1 > 6 || y1 < 0 || y1 > 6) {
                throw std::invalid_argument("Invalid move. (" + san + ")");
            }

            if (x2 < 0 || x2 > 6 || y2 < 0 || y2 > 6) {
                throw std::invalid_argument("Invalid move. (" + san + ")");
            }

            const int sq1 = 7 * y1 + x1;
            const int sq2 = 7 * y2 + x2;
            const int dx = std::abs(x1 - x2);
            const int dy = std::abs(y1 - y2);

            // We were just given a single jump in longhand notation
            // e.g. "b2b3" instead of "b3"
            if (dx <= 1 && dy <= 1) {
                return Move(static_cast<Square>(sq2));
            }
            // Normal double jump
            else {
                return Move(static_cast<Square>(sq1), static_cast<Square>(sq2));
            }
        } else if (san == "0000" || san == "null") {
            return Move(Square::a1);
        } else {
            throw std::invalid_argument("Invalid length (" + san + ")");
        }
    }

   private:
    const Square to_;
    const Square from_;
};

inline bool operator==(const Move &lhs, const Move &rhs) {
    return lhs.to() == rhs.to() && lhs.from() == rhs.from();
}

inline bool operator!=(const Move &lhs, const Move &rhs) {
    return lhs.to() != rhs.to() || lhs.from() != rhs.from();
}

inline std::ostream &operator<<(std::ostream &os, const Move &m) {
    const Square from = m.from();
    const Square to = m.to();
    if (m.type() == MoveType::Double) {
        os << static_cast<char>(sq_to_file(from) + 'a')
           << static_cast<char>(sq_to_rank(from) + '1');
    }
    os << static_cast<char>(sq_to_file(to) + 'a')
       << static_cast<char>(sq_to_rank(to) + '1');
    return os;
}

}  // namespace libataxx

#endif
