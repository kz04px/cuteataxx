#ifndef LIBATAXX_CONSTANTS_HPP
#define LIBATAXX_CONSTANTS_HPP

#include <cstdint>

namespace libataxx {

#define MAX_MOVES 256

enum Bitboards : std::uint64_t
{
    All = 0x1FFFFFFFFFFFFULL,
    FileA = 0x0040810204081ULL,
    FileB = 0x0081020408102ULL,
    FileC = 0x0102040810204ULL,
    FileD = 0x0204081020408ULL,
    FileE = 0x0408102040810ULL,
    FileF = 0x0810204081020ULL,
    FileG = 0x1020408102040ULL,
    NotFileA = 0x1fbf7efdfbf7eULL,
    NotFileB = 0x1f7efdfbf7efdULL,
    NotFileC = 0x1efdfbf7efdfbULL,
    NotFileD = 0x1dfbf7efdfbf7ULL,
    NotFileE = 0x1bf7efdfbf7efULL,
    NotFileF = 0x17efdfbf7efdfULL,
    NotFileG = 0x0fdfbf7efdfbfULL
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

const std::uint64_t dist2[49] = {
    0x1c204,         0x3c408,         0x7c891,         0xf9122,
    0x1f2244,        0x1e0408,        0x1c0810,        0xe10204,
    0x1e20408,       0x3e44891,       0x7c89122,       0xf912244,
    0xf020408,       0xe040810,       0x70810207,      0xf102040f,
    0x1f224489f,     0x3e448913e,     0x7c891227c,     0x781020478,
    0x702040870,     0x3840810380,    0x7881020780,    0xf912244f80,
    0x1f224489f00,   0x3e448913e00,   0x3c081023c00,   0x38102043800,
    0x1c204081c000,  0x3c408103c000,  0x7c891227c000,  0xf912244f8000,
    0x1f224489f0000, 0x1e040811e0000, 0x1c081021c0000, 0x102040e00000,
    0x204081e00000,  0x448913e00000,  0x891227c00000,  0x112244f800000,
    0x20408f000000,  0x40810e000000,  0x102070000000,  0x2040f0000000,
    0x4489f0000000,  0x8913e0000000,  0x11227c0000000, 0x204780000000,
    0x408700000000};

}  // namespace libataxx

#endif
