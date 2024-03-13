#include "core/ataxx/adjudicate.hpp"
#include <doctest/doctest.h>
#include <array>
#include <iostream>
#include <libataxx/move.hpp>
#include <libataxx/position.hpp>
#include <string>

TEST_CASE("Adjudicate - easyfill true") {
    const std::array fens = {
        "xoo4/ooo4/ooo4/7/7/7/7 x 0 1",
        "oxx4/xxx4/xxx4/7/7/7/7 o 0 1",
        "x--4/ooo4/ooo4/7/7/7/7 x 0 1",
        "o--4/xxx4/xxx4/7/7/7/7 o 0 1",
        "xxxxxxx/ooooooo/ooooooo/7/7/7/7 x 0 1",
        "x------/-------/-------/-------/-------/-------/o1----- x 0 1",
        // Tough ones
        "x------/-------/-------/-------/-------/-------/o-2--- x 0 1",
        "x------/-------/-------/-------/-------/-------/1-o1--- x 0 1",
        "x------/-------/-------/-------/-------/-------/1-1o--- x 0 1",
        "x------/-------/-------/7/-------/-------/o-2--- x 0 1",
        "x------/----1--/------1/-2----/------1/-------/o-1-1-- x 0 1",
    };

    for (const auto &fen : fens) {
        const auto pos = libataxx::Position(fen);
        REQUIRE(can_adjudicate_easyfill(pos));
    }
}

TEST_CASE("Adjudicate - easyfill false") {
    const std::array fens = {
        "startpos",
        "7/7/7/7/7/7/7 x 0 1",
        "7/7/7/7/7/7/7 o 0 1",
        "xoo4/o1o4/ooo4/7/7/7/7 x 0 1",
        "oxx4/x1x4/xxx4/7/7/7/7 o 0 1",
        "xxxxxxx/ooooooo/7/7/7/7/7 x 0 1",
        "xxxxxxx/xxxxxxx/xxxxxxx/xxxxxxx/ooooooo/ooooooo/7 x 0 1",
        "x------/-------/-------/-------/-------/-------/o-1---- x 0 1",
        "xxx----/-------/-------/-------/-------/-------/o1----- x 0 1",
        "xoo4/ooo4/ooo4/7/7/7/7 x 100 1",
        "oxx4/xxx4/xxx4/7/7/7/7 o 100 1",
        "xo-4/-------/-------/-------/-------/-------/------- x 0 1",
        "x------/-------/-------/7/-------/-------/o------ x 0 1",
        "xx-----/-------/-------/7/-------/-------/o------ x 0 1",
        "x------/-------/-------/7/-------/-------/oo----- x 0 1",
        "x------/-------/-------/2-----/-------/-------/o-1--- x 0 1",
    };

    for (const auto &fen : fens) {
        const auto pos = libataxx::Position(fen);
        REQUIRE(!can_adjudicate_easyfill(pos));
    }
}
