#include "core/ataxx/parse_move.hpp"
#include <doctest/doctest.h>
#include <array>
#include <libataxx/move.hpp>
#include <string>

TEST_CASE("Parse - null/pass") {
    const std::array<std::string, 8> tests = {
        "0000",
        "null",
        "NULL",
        "a1a1",
        "g7g7",
        "g7G7",
        "G7g7",
        "G7G7",
    };

    for (const auto &str : tests) {
        const auto move = parse_move(str);
        REQUIRE(move == libataxx::Move::nullmove());
    }
}

TEST_CASE("Parse - singles") {
    const std::array<std::pair<std::string, libataxx::Move>, 10> tests = {{
        {"a1", libataxx::Move(libataxx::SquareIndex::A1)},
        {"a7", libataxx::Move(libataxx::SquareIndex::A7)},
        {"g1", libataxx::Move(libataxx::SquareIndex::G1)},
        {"g7", libataxx::Move(libataxx::SquareIndex::G7)},
        {"x@a1", libataxx::Move(libataxx::SquareIndex::A1)},
        {"X@a1", libataxx::Move(libataxx::SquareIndex::A1)},
        {"a1a2", libataxx::Move(libataxx::SquareIndex::A2)},
        {"A1", libataxx::Move(libataxx::SquareIndex::A1)},
        {"x@A2", libataxx::Move(libataxx::SquareIndex::A2)},
        {"X@A3", libataxx::Move(libataxx::SquareIndex::A3)},
    }};

    for (const auto &[str, result] : tests) {
        const auto move = parse_move(str);
        REQUIRE(move == result);
    }
}

TEST_CASE("Parse - doubles") {
    const std::array<std::pair<std::string, libataxx::Move>, 4> tests = {{
        {"a1a3", libataxx::Move(libataxx::SquareIndex::A1, libataxx::SquareIndex::A3)},
        {"a1A3", libataxx::Move(libataxx::SquareIndex::A1, libataxx::SquareIndex::A3)},
        {"A1a3", libataxx::Move(libataxx::SquareIndex::A1, libataxx::SquareIndex::A3)},
        {"A1A3", libataxx::Move(libataxx::SquareIndex::A1, libataxx::SquareIndex::A3)},
    }};

    for (const auto &[str, result] : tests) {
        const auto move = parse_move(str);
        REQUIRE(move == result);
    }
}

TEST_CASE("Parse - invalid") {
    const std::array<std::string, 12> tests = {
        "",
        "test",
        "longstring"
        "    "
        "none"
        "~~",
        "a1a4",
        "a0",
        "a8",
        "g0",
        "g8",
        "h1",
        // "aaaa",
        // "1111",
    };

    for (const auto &str : tests) {
        INFO(str);
        REQUIRE_THROWS(parse_move(str));
    }
}
