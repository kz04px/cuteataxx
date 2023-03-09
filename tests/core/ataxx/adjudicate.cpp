#include "core/ataxx/adjudicate.hpp"
#include <doctest/doctest.h>
#include <array>
#include <libataxx/move.hpp>
#include <libataxx/position.hpp>
#include <string>

TEST_CASE("Adjudicate - easyfill true") {
    const std::array<std::string, 4> fens = {
        "xoo4/ooo4/ooo4/7/7/7/7 x 0 1",
        "oxx4/xxx4/xxx4/7/7/7/7 o 0 1",
        "x--4/ooo4/ooo4/7/7/7/7 x 0 1",
        "o--4/xxx4/xxx4/7/7/7/7 o 0 1",
    };

    for (const auto &fen : fens) {
        const auto pos = libataxx::Position(fen);
        REQUIRE(can_adjudicate_easyfill(pos));
    }
}

TEST_CASE("Adjudicate - easyfill false") {
    const std::array<std::string, 5> fens = {
        "startpos",
        "7/7/7/7/7/7/7 x 0 1",
        "7/7/7/7/7/7/7 o 0 1",
        "xoo4/o1o4/ooo4/7/7/7/7 x 0 1",
        "oxx4/x1x4/xxx4/7/7/7/7 o 0 1",
    };

    for (const auto &fen : fens) {
        const auto pos = libataxx::Position(fen);
        REQUIRE(!can_adjudicate_easyfill(pos));
    }
}
