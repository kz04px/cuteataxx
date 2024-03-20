#ifndef TOURNAMENT_ROUNDROBIN_MIXED_HPP
#define TOURNAMENT_ROUNDROBIN_MIXED_HPP

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <vector>
#include "generator.hpp"

class [[nodiscard]] RoundRobinMixedGenerator final : public TournamentGenerator {
   public:
    RoundRobinMixedGenerator(const std::size_t players,
                             const std::size_t games,
                             const std::size_t openings,
                             const bool r)
        : num_players(players), num_games(games), num_openings(openings), repeat(r) {
        magic.emplace_back(0);
        for (std::size_t i = 2; i < players + (players % 2); ++i) {
            magic.emplace_back(i);
        }
        magic.emplace_back(1);

        player1 = 0;
        player2 = 1;
        opening = num_openings - 1;
    }

    virtual ~RoundRobinMixedGenerator() {
    }

    [[nodiscard]] virtual auto is_finished() -> bool override {
        return idx >= expected();
    }

    [[nodiscard]] virtual auto expected() -> std::size_t override {
        const auto games_per_player = num_games * (num_players - 1);
        return games_per_player * num_players / 2;
    }

    [[nodiscard]] virtual auto next() -> GameInfo override {
        increment();
        const auto is_mirror = repeat && idx % 2 == 0;
        if (is_mirror) {
            return GameInfo{idx - 1, opening, magic.at(player2), magic.at(player1)};
        } else {
            return GameInfo{idx - 1, opening, magic.at(player1), magic.at(player2)};
        }
    }

   private:
    virtual auto increment() -> void override {
        if (repeat) {
            if (idx % 2 == 0) {
                player1++;
                player2--;
                opening++;
            }
        } else {
            player1++;
            player2--;
            opening++;
        }

        auto okay = true;
        do {
            okay = true;

            // New round?
            if (player1 >= player2) {
                std::rotate(magic.rbegin(), magic.rbegin() + 1, magic.rend() - 1);
                player1 = 0;
                player2 = magic.size() - 1;
                okay = false;
            }

            // Bye?
            const auto is_bye = magic.at(player1) == num_players || magic.at(player2) == num_players;
            if (is_bye) {
                player1++;
                player2--;
                okay = false;
            }
        } while (!okay);

        opening = opening % num_openings;

        idx++;
    }

    std::size_t num_players = 0;
    std::size_t num_games = 0;
    std::size_t num_openings = 0;
    std::size_t idx = 0;
    std::size_t opening = 0;
    std::size_t player1 = 0;
    std::size_t player2 = 1;
    std::vector<std::size_t> magic;
    bool repeat = true;
};

#endif
