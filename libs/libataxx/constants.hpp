#ifndef LIBATAXX_CONSTANTS_HPP
#define LIBATAXX_CONSTANTS_HPP

#include <cstdint>

namespace libataxx {

const int max_moves = 256;

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

namespace zobrist {

const std::uint64_t turn = 0x2e98304a94e1000d;
const std::uint64_t piece[2][49] = {
    {0xddd67db865dc92f9, 0x31ad7f3d49884764, 0xfc810e82600d77ed,
     0xa329bc2fe9a585c2, 0x0dd7013c7b5f9ee0, 0xcbfb18e330c5152b,
     0x5ca13c8237e969f0, 0xcfe82be3298f4860, 0xd74ee79ab8cd59d4,
     0x76c9804b3dd3dd9a, 0x62fe43ac3416344b, 0x84f44cfc5640f1fb,
     0xee80f44f274d194f, 0x9e132307eaa1062a, 0x8ef3b83e05104b8c,
     0x7458cb4e38a56b1b, 0x496ae857824032b5, 0x297892ab4aed9139,
     0x2c63a6ed1e5c503b, 0xb806e46b057da75c, 0x8e3ee3fc5ea21f4d,
     0x7bb71073c6b32631, 0xa34cbc35fea7986d, 0x2cf34121f29c2073,
     0x3ff14ac1cfb3ac44, 0x9a1f60c67467b9eb, 0x4f69347f2d665427,
     0x4d51d73c213217bc, 0x6b3089c84d5d78e3, 0xa2923e3c0bbc0f50,
     0xe4931d122c14c947, 0xa1b03c639fe421a7, 0x2789a4dc1574310c,
     0x4e3f96a633e0ede0, 0xe4f29bf5ff2869df, 0x72e7fe1c77af10b3,
     0x245e4d0677acbd90, 0x5eb16a0a6cff1f55, 0x26e93216b330c067,
     0x0eced566d7522ad3, 0xdc64bc04aa39e421, 0x401987ae3cb114db,
     0xd0d0b88ece5f0c16, 0x13275209f4ddff5a, 0xffb20d8b83a2ce28,
     0x8f2100bf98b54217, 0x3769a2d70b7209e7, 0x61b3cca215f1a975,
     0x8d1158e421453a0f},
    {0xd33fd8453c18380c, 0xd7949af7fa5e24ca, 0xa75e294be7913781,
     0x3b615f1e3322be8c, 0xe4c885ecb48d6187, 0x9b92f432a8d105f3,
     0xbff867477fb93a23, 0x5ff3e0b5e6e72ab4, 0xe60144f2900bc655,
     0xf29a0de00bdc3aee, 0xde9e042c06230fd8, 0x4264ec62f70ba708,
     0x5f175b53ebb138c7, 0x439e71025499179e, 0xcf39b10d6a8437fd,
     0x0fcf1fc30f9e0de4, 0x6a502645f6a31fef, 0xd47dd00cf21ddfdd,
     0x7e4ca0ec0c2e7f68, 0x37401f0941e5a469, 0x3efddb557fd01cb4,
     0x846bfb50b6965c90, 0xc0fa4a424c735605, 0xe5fbff674c562239,
     0x3826959008ef40f0, 0x4a712b5964a4b7a1, 0x68f594a0740c456f,
     0x1ee9e39d56bd32e6, 0x08ebd76592898d31, 0x3df7c91613fb946d,
     0xef3e760a57d5a399, 0x6ef3128154e16b88, 0x15e27065e3d4c108,
     0xd5fe096a16b5eed2, 0xb2964fa8f1bddad1, 0x3ebb6f3af8a61dc5,
     0x9776dc9ebc5243d2, 0x4660bff8b53c4a78, 0x73507ecb48163932,
     0x05967644e76ae2f8, 0x9c53ce9f0fdcb066, 0xcf6e214c6acea558,
     0x53ee7540169475d8, 0xc8a49004b87946b8, 0x1d75d48ff9dc94b2,
     0xec4f8821305bbd96, 0x43ed7506a2bd6b16, 0xf0eb5d0784e3eb03,
     0xe578cac94b342a86}};

}  // namespace zobrist

}  // namespace libataxx

#endif
