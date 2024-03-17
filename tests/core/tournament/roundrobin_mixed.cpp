#include "core/tournament/roundrobin_mixed.hpp"
#include <doctest/doctest.h>

TEST_SUITE("Tournament - Round Robin Mixed") {
    TEST_CASE("Test 1") {
        const auto num_players = 2;
        const auto num_games = 2;
        const auto num_openings = 2;
        auto gen = RoundRobinMixedGenerator(num_players, num_games, num_openings, true);

        REQUIRE(gen.expected() == 2);

        // id, opening, player1, player2
        REQUIRE(gen.next() == GameInfo{0, 0, 0, 1});
        REQUIRE(gen.next() == GameInfo{1, 0, 1, 0});

        // Overflow
        REQUIRE(gen.next() == GameInfo{2, 1, 0, 1});
        REQUIRE(gen.next() == GameInfo{3, 1, 1, 0});

        // Extra overflow
        REQUIRE(gen.next() == GameInfo{4, 0, 0, 1});
        REQUIRE(gen.next() == GameInfo{5, 0, 1, 0});
        REQUIRE(gen.next() == GameInfo{6, 1, 0, 1});
        REQUIRE(gen.next() == GameInfo{7, 1, 1, 0});
        REQUIRE(gen.next() == GameInfo{8, 0, 0, 1});
        REQUIRE(gen.next() == GameInfo{9, 0, 1, 0});
    }

    TEST_CASE("Test 2") {
        const auto num_players = 2;
        const auto num_games = 4;
        const auto num_openings = 2;
        auto gen = RoundRobinMixedGenerator(num_players, num_games, num_openings, true);

        REQUIRE(gen.expected() == 4);

        // id, opening, player1, player2
        REQUIRE(gen.next() == GameInfo{0, 0, 0, 1});
        REQUIRE(gen.next() == GameInfo{1, 0, 1, 0});
        REQUIRE(gen.next() == GameInfo{2, 1, 0, 1});
        REQUIRE(gen.next() == GameInfo{3, 1, 1, 0});

        // Overflow
        REQUIRE(gen.next() == GameInfo{4, 0, 0, 1});
        REQUIRE(gen.next() == GameInfo{5, 0, 1, 0});
        REQUIRE(gen.next() == GameInfo{6, 1, 0, 1});
        REQUIRE(gen.next() == GameInfo{7, 1, 1, 0});
    }

    TEST_CASE("Test 3") {
        const auto num_players = 4;
        const auto num_games = 2;
        const auto num_openings = 2;
        auto gen = RoundRobinMixedGenerator(num_players, num_games, num_openings, true);

        REQUIRE(gen.expected() == 12);

        // id, opening, player1, player2
        REQUIRE(gen.next() == GameInfo{0, 0, 0, 3});
        REQUIRE(gen.next() == GameInfo{1, 0, 3, 0});
        REQUIRE(gen.next() == GameInfo{2, 1, 1, 2});
        REQUIRE(gen.next() == GameInfo{3, 1, 2, 1});

        REQUIRE(gen.next() == GameInfo{4, 0, 0, 2});
        REQUIRE(gen.next() == GameInfo{5, 0, 2, 0});
        REQUIRE(gen.next() == GameInfo{6, 1, 3, 1});
        REQUIRE(gen.next() == GameInfo{7, 1, 1, 3});

        REQUIRE(gen.next() == GameInfo{8, 0, 0, 1});
        REQUIRE(gen.next() == GameInfo{9, 0, 1, 0});
        REQUIRE(gen.next() == GameInfo{10, 1, 2, 3});
        REQUIRE(gen.next() == GameInfo{11, 1, 3, 2});

        // Overflow
        REQUIRE(gen.next() == GameInfo{12, 0, 0, 3});
        REQUIRE(gen.next() == GameInfo{13, 0, 3, 0});
        REQUIRE(gen.next() == GameInfo{14, 1, 1, 2});
        REQUIRE(gen.next() == GameInfo{15, 1, 2, 1});
        REQUIRE(gen.next() == GameInfo{16, 0, 0, 2});
        REQUIRE(gen.next() == GameInfo{17, 0, 2, 0});
        REQUIRE(gen.next() == GameInfo{18, 1, 3, 1});
        REQUIRE(gen.next() == GameInfo{19, 1, 1, 3});
    }

    TEST_CASE("Test 4") {
        const auto num_players = 4;
        const auto num_games = 4;
        const auto num_openings = 4;
        auto gen = RoundRobinMixedGenerator(num_players, num_games, num_openings, true);

        REQUIRE(gen.expected() == 24);

        // id, opening, player1, player2
        REQUIRE(gen.next() == GameInfo{0, 0, 0, 3});
        REQUIRE(gen.next() == GameInfo{1, 0, 3, 0});
        REQUIRE(gen.next() == GameInfo{2, 1, 1, 2});
        REQUIRE(gen.next() == GameInfo{3, 1, 2, 1});

        REQUIRE(gen.next() == GameInfo{4, 2, 0, 2});
        REQUIRE(gen.next() == GameInfo{5, 2, 2, 0});
        REQUIRE(gen.next() == GameInfo{6, 3, 3, 1});
        REQUIRE(gen.next() == GameInfo{7, 3, 1, 3});

        REQUIRE(gen.next() == GameInfo{8, 0, 0, 1});
        REQUIRE(gen.next() == GameInfo{9, 0, 1, 0});
        REQUIRE(gen.next() == GameInfo{10, 1, 2, 3});
        REQUIRE(gen.next() == GameInfo{11, 1, 3, 2});

        REQUIRE(gen.next() == GameInfo{12, 2, 0, 3});
        REQUIRE(gen.next() == GameInfo{13, 2, 3, 0});
        REQUIRE(gen.next() == GameInfo{14, 3, 1, 2});
        REQUIRE(gen.next() == GameInfo{15, 3, 2, 1});

        REQUIRE(gen.next() == GameInfo{16, 0, 0, 2});
        REQUIRE(gen.next() == GameInfo{17, 0, 2, 0});
        REQUIRE(gen.next() == GameInfo{18, 1, 3, 1});
        REQUIRE(gen.next() == GameInfo{19, 1, 1, 3});

        REQUIRE(gen.next() == GameInfo{20, 2, 0, 1});
        REQUIRE(gen.next() == GameInfo{21, 2, 1, 0});
        REQUIRE(gen.next() == GameInfo{22, 3, 2, 3});
        REQUIRE(gen.next() == GameInfo{23, 3, 3, 2});

        // Overflow
        REQUIRE(gen.next() == GameInfo{24, 0, 0, 3});
        REQUIRE(gen.next() == GameInfo{25, 0, 3, 0});
    }

    TEST_CASE("Test no repeat") {
        const auto num_players = 2;
        const auto num_games = 2;
        const auto num_openings = 2;
        auto gen = RoundRobinMixedGenerator(num_players, num_games, num_openings, false);

        REQUIRE(gen.expected() == 2);

        // id, opening, player1, player2
        REQUIRE(gen.next() == GameInfo{0, 0, 0, 1});
        REQUIRE(gen.next() == GameInfo{1, 1, 0, 1});

        // Overflow
        REQUIRE(gen.next() == GameInfo{2, 0, 0, 1});
        REQUIRE(gen.next() == GameInfo{3, 1, 0, 1});

        // Extra overflow
        REQUIRE(gen.next() == GameInfo{4, 0, 0, 1});
        REQUIRE(gen.next() == GameInfo{5, 1, 0, 1});
        REQUIRE(gen.next() == GameInfo{6, 0, 0, 1});
        REQUIRE(gen.next() == GameInfo{7, 1, 0, 1});
        REQUIRE(gen.next() == GameInfo{8, 0, 0, 1});
        REQUIRE(gen.next() == GameInfo{9, 1, 0, 1});
    }

    TEST_CASE("Test no repeat 2") {
        const auto num_players = 6;
        const auto num_games = 2;
        const auto num_openings = 2;
        auto gen = RoundRobinMixedGenerator(num_players, num_games, num_openings, false);

        REQUIRE(gen.expected() == 30);

        // id, opening, player1, player2
        REQUIRE(gen.next() == GameInfo{0, 0, 0, 5});
        REQUIRE(gen.next() == GameInfo{1, 1, 1, 4});
        REQUIRE(gen.next() == GameInfo{2, 0, 2, 3});

        REQUIRE(gen.next() == GameInfo{3, 1, 0, 4});
        REQUIRE(gen.next() == GameInfo{4, 0, 5, 3});
        REQUIRE(gen.next() == GameInfo{5, 1, 1, 2});

        REQUIRE(gen.next() == GameInfo{6, 0, 0, 3});
        REQUIRE(gen.next() == GameInfo{7, 1, 4, 2});
        REQUIRE(gen.next() == GameInfo{8, 0, 5, 1});

        REQUIRE(gen.next() == GameInfo{9, 1, 0, 2});
        REQUIRE(gen.next() == GameInfo{10, 0, 3, 1});
        REQUIRE(gen.next() == GameInfo{11, 1, 4, 5});

        REQUIRE(gen.next() == GameInfo{12, 0, 0, 1});
        REQUIRE(gen.next() == GameInfo{13, 1, 2, 5});
        REQUIRE(gen.next() == GameInfo{14, 0, 3, 4});

        REQUIRE(gen.next() == GameInfo{15, 1, 0, 5});
        REQUIRE(gen.next() == GameInfo{16, 0, 1, 4});
        REQUIRE(gen.next() == GameInfo{17, 1, 2, 3});

        REQUIRE(gen.next() == GameInfo{18, 0, 0, 4});
        REQUIRE(gen.next() == GameInfo{19, 1, 5, 3});
        REQUIRE(gen.next() == GameInfo{20, 0, 1, 2});

        REQUIRE(gen.next() == GameInfo{21, 1, 0, 3});
        REQUIRE(gen.next() == GameInfo{22, 0, 4, 2});
        REQUIRE(gen.next() == GameInfo{23, 1, 5, 1});

        REQUIRE(gen.next() == GameInfo{24, 0, 0, 2});
        REQUIRE(gen.next() == GameInfo{25, 1, 3, 1});
        REQUIRE(gen.next() == GameInfo{26, 0, 4, 5});

        REQUIRE(gen.next() == GameInfo{27, 1, 0, 1});
        REQUIRE(gen.next() == GameInfo{28, 0, 2, 5});
        REQUIRE(gen.next() == GameInfo{29, 1, 3, 4});
    }
}
