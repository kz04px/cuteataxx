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

}  // namespace libataxx

#endif
