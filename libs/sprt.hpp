#ifndef SPRT_HPP
#define SPRT_HPP

#include <cmath>
#include <tuple>

namespace {

[[nodiscard]] constexpr auto elo_to_probability(const float elo, const float drawelo)
    -> std::tuple<float, float, float> {
    const auto pwin = 1.0f / (1.0f + std::pow(10.0f, (-elo + drawelo) / 400.0f));
    const auto ploss = 1.0f / (1.0f + std::pow(10.0f, (elo + drawelo) / 400.0f));
    const auto pdraw = 1.0f - pwin - ploss;
    return {pwin, pdraw, ploss};
}

[[nodiscard]] constexpr auto probability_to_elo(const float pwin, const float pdraw, const float ploss)
    -> std::pair<float, float> {
    const auto elo = 200.0f * std::log10(pwin / ploss * (1.0f - ploss) / (1.0f - pwin));
    const auto draw_elo = 200.0f * std::log10((1.0f - ploss) / ploss * (1.0f - pwin) / pwin);
    return {elo, draw_elo};
}

}  // namespace

namespace sprt {

[[nodiscard]] constexpr auto get_llr(int wins, int losses, int draws, const float elo0, const float elo1) -> float {
    wins = std::max(wins, 1);
    losses = std::max(losses, 1);
    draws = std::max(draws, 1);

    const auto total = wins + losses + draws;

    const auto [elo, drawelo] = probability_to_elo(float(wins) / total, float(draws) / total, float(losses) / total);

    const auto [p0win, p0draw, p0loss] = elo_to_probability(elo0, drawelo);
    const auto [p1win, p1draw, p1loss] = elo_to_probability(elo1, drawelo);

    const auto wins_factor = wins * std::log(p1win / p0win);
    const auto losses_factor = losses * std::log(p1loss / p0loss);
    const auto draws_factor = draws * std::log(p1draw / p0draw);

    return wins_factor + losses_factor + draws_factor;
}

[[nodiscard]] constexpr auto get_lbound(const float alpha, const float beta) -> float {
    return std::log(beta / (1.0f - alpha));
}

[[nodiscard]] constexpr auto get_ubound(const float alpha, const float beta) -> float {
    return std::log((1.0f - beta) / alpha);
}

static_assert(std::round(get_llr(0, 0, 0, -10, 10) * 100) / 100 == 0.0f);
static_assert(std::round(get_llr(10, 10, 10, -10, 10) * 100) / 100 == 0.0f);
static_assert(std::round(get_llr(3415, 3270, 5763, -1, 4) * 100) / 100 == 2.16f);
static_assert(std::round(get_llr(4413, 4218, 7481, -1, 4) * 100) / 100 == 2.96f);
static_assert(std::round(get_llr(1382, 1415, 2627, 0, 5) * 100) / 100 == -1.34f);
static_assert(std::round(get_llr(7238, 7273, 18473, 0, 4) * 100) / 100 == -2.97f);
static_assert(std::round(get_llr(7446, 7503, 14227, -3, 1) * 100) / 100 == 0.12f);

static_assert(std::round(get_lbound(0.05f, 0.05f) * 100) / 100 == -2.94f);
static_assert(std::round(get_lbound(0.01f, 0.01f) * 100) / 100 == -4.60f);

static_assert(std::round(get_ubound(0.05f, 0.05f) * 100) / 100 == 2.94f);
static_assert(std::round(get_ubound(0.01f, 0.01f) * 100) / 100 == 4.60f);

}  // namespace sprt

#endif
