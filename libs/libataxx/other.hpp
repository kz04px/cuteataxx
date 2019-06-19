#ifndef LIBATAXX_OTHER_HPP
#define LIBATAXX_OTHER_HPP

#include <cassert>
#include <cstdint>
#include "constants.hpp"

namespace libataxx {

inline int sq_to_rank(const int sq) {
    return sq / 7;
}

inline int sq_to_rank(const Square sq) {
    return static_cast<int>(sq) / 7;
}

inline int sq_to_file(const int sq) {
    return sq % 7;
}

inline int sq_to_file(const Square sq) {
    return static_cast<int>(sq) % 7;
}

inline std::uint64_t sq_to_bb(const int sq) {
    return 1ULL << static_cast<int>(sq);
}

inline std::uint64_t sq_to_bb(const Square sq) {
    return 1ULL << static_cast<int>(sq);
}

inline int lsbll(const std::uint64_t bb) {
    assert(bb);
    return __builtin_ctzll(bb);
}

inline int popcountll(const std::uint64_t bb) {
    return __builtin_popcountll(bb);
}

std::uint64_t single_moves(const std::uint64_t bb) {
    return Bitboards::All &
           (((bb >> 1) & (Bitboards::NotFileG)) |  // Left 1
            ((bb << 1) & (Bitboards::NotFileA)) |  // Right 1
            (bb << 7) |                            // Up 1
            (bb >> 7) |                            // Down 1
            ((bb << 8) & (Bitboards::NotFileA)) |  // Up 1 right 1
            ((bb << 6) & (Bitboards::NotFileG)) |  // Up 1 left 1
            ((bb >> 8) & (Bitboards::NotFileG)) |  // Down 1 left 1
            ((bb >> 6) & (Bitboards::NotFileA)));  // Down 1 right 1
}

std::uint64_t double_moves(int sq) {
    assert(0 <= sq && sq <= 48);
    return dist2[sq];
}

}  // namespace libataxx

#endif
