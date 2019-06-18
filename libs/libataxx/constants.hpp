#ifndef LIBATAXX_CONSTANTS_HPP
#define LIBATAXX_CONSTANTS_HPP

#include <cstdint>

namespace libataxx {

enum Bitboards : std::uint64_t
{
    All = 0x1FFFFFFFFFFFFULL,
};

// clang-format off
enum Square : std::uint8_t
{
    a1 = 0, b1, c1, d1, e1, f1, g1,
    a2, b2, c2, d2, e2, f2, g2,
    a3, b3, c3, d3, e3, f3, g3,
    a4, b4, c4, d4, e4, f4, g4,
    a5, b5, c5, d5, e5, f5, g5,
    a6, b6, c6, d6, e6, f6, g6,
    a7, b7, c7, d7, e7, f7, g7,
    None
};
// clang-format on

}  // namespace libataxx

#endif
