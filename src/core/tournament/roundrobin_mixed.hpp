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
        for (std::size_t i = 0; i < players; ++i) {
            magic.emplace_back(i);
        }
        player1 = 0;
        player2 = num_players - 1;
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
        const auto result = GameInfo{idx, opening, player1, player2};
        increment();
        return result;
    }

   private:
    virtual auto increment() -> void override {
        idx++;

        if (repeat) {
            const auto is_new_round = idx % (num_players) == 0;
            if (is_new_round) {
                std::rotate(magic.rbegin(), magic.rbegin() + 1, magic.rend() - 1);
            }

            if (idx % 2) {
                player1 = magic.at(num_players - 1 - ((idx / 2) % (num_players / 2)));
                player2 = magic.at((idx / 2) % (num_players / 2));
            } else {
                player1 = magic.at((idx / 2) % (num_players / 2));
                player2 = magic.at(num_players - 1 - ((idx / 2) % (num_players / 2)));
                opening++;
            }
        } else {
            const auto is_new_round = idx % (num_players / 2) == 0;
            if (is_new_round) {
                std::rotate(magic.rbegin(), magic.rbegin() + 1, magic.rend() - 1);
            }

            player1 = magic.at(idx % (num_players / 2));
            player2 = magic.at(num_players - 1 - (idx % (num_players / 2)));
            opening++;
        }

        opening = opening % num_openings;

        assert(player1 < num_players);
        assert(player2 < num_players);
    }

    std::size_t num_players = 0;
    std::size_t num_games = 0;
    std::size_t num_openings = 0;
    bool repeat = true;
    // state
    std::size_t idx = 0;
    std::size_t opening = 0;
    std::size_t player1 = 0;
    std::size_t player2 = 1;
    std::size_t match_game = 0;
    std::vector<std::size_t> magic;
};

#endif
