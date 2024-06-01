#ifndef CUTEATAXX_TUI_TABS_GAME_HPP
#define CUTEATAXX_TUI_TABS_GAME_HPP

#include <algorithm>
#include <libataxx/position.hpp>
#include <map>
#include <string>
#include <vector>
#include "base.hpp"
// FTXUI
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
// Core
#include "core/match/results.hpp"
#include "core/parse/settings.hpp"
// Components
#include "../components/board.hpp"
#include "../components/clock.hpp"
#include "../components/history.hpp"
#include "../components/material_bar.hpp"

[[nodiscard]] auto get_sorted(const std::map<std::string, Score> &scores)
    -> std::vector<std::pair<std::string, Score>> {
    using pair_type = std::pair<std::string, Score>;
    auto sorted = std::vector<pair_type>();

    for (const auto &[name, score] : scores) {
        sorted.emplace_back(std::make_pair(name, score));
    }

    std::sort(std::begin(sorted), std::end(sorted), [](const pair_type &lhs, const pair_type &rhs) {
        const auto score_lhs = 2 * std::get<1>(lhs).wins + std::get<1>(lhs).draws;
        const auto score_rhs = 2 * std::get<1>(rhs).wins + std::get<1>(rhs).draws;
        return score_lhs > score_rhs;
    });

    return sorted;
}

[[nodiscard]] auto get_lines(const std::vector<std::pair<std::string, Score>> &sorted)
    -> std::vector<std::vector<std::string>> {
    std::vector<std::vector<std::string>> lines;
    lines.emplace_back(std::vector<std::string>{"Engine", "Wins", "Losses", "Draws", "Played", "Score"});

    for (const auto &[name, score] : sorted) {
        const auto points_string = std::to_string(score.wins + score.draws / 2) + (score.draws % 2 ? ".5" : ".0");
        lines.emplace_back(std::vector<std::string>{name,
                                                    std::to_string(score.wins),
                                                    std::to_string(score.losses),
                                                    std::to_string(score.draws),
                                                    std::to_string(score.played),
                                                    points_string});
    }

    return lines;
}

class [[nodiscard]] GameTab : public BaseTab {
   public:
    GameTab() {
        m_clock_black.title = "black";
        m_clock_black.colour = Color::Black;
        m_clock_white.title = "white";
        m_clock_white.colour = Color::White;
    }

    [[nodiscard]] auto render() -> Component {
        return Renderer([this] {
            const auto sorted = get_sorted(m_results.scores);
            const auto lines = get_lines(sorted);
            auto table = Table(lines);

            table.SelectRow(0).Border(LIGHT);
            table.SelectColumn(0).Border(LIGHT);
            table.SelectColumn(1).Border(LIGHT);
            table.SelectColumn(2).Border(LIGHT);
            table.SelectColumn(3).Border(LIGHT);
            table.SelectColumn(4).Border(LIGHT);
            table.SelectColumn(5).Border(LIGHT);
            table.SelectColumn(1).DecorateCells(align_right);
            table.SelectColumn(2).DecorateCells(align_right);
            table.SelectColumn(3).DecorateCells(align_right);
            table.SelectColumn(4).DecorateCells(align_right);
            table.SelectColumn(5).DecorateCells(align_right);

            // Colouring
            auto content = table.SelectRows(1, -1);
            content.DecorateCellsAlternateRow(color(Color::Blue), 2, 0);
            content.DecorateCellsAlternateRow(color(Color::Cyan), 2, 1);

            return hbox({
                vbox({
                    text(m_title) | center,
                    m_board.render(),
                    m_material_bar.render(),
                }),
                vbox({
                    m_clock_white.render(),
                    m_history.render(),
                    m_clock_black.render(),
                }),
                table.Render(),
            });
        });
    }

    auto new_game() -> void {
        m_history.moves.clear();
    }

    auto set_position(const std::string &fen) -> void {
        m_board.pos.set_fen(fen);
        if (m_board.pos.get_turn() == libataxx::Side::Black) {
            m_clock_black.ticking = true;
            m_clock_white.ticking = false;
            m_history.white_first = false;
        } else {
            m_clock_black.ticking = false;
            m_clock_white.ticking = true;
            m_history.white_first = true;
        }
        m_material_bar.num_black = m_board.pos.get_black().count();
        m_material_bar.num_white = m_board.pos.get_white().count();
    }

    auto set_title(const std::string &title) -> void {
        m_title = title;
    }

    auto set_clock(const SearchSettings &tc) -> void {
        m_clock_black.time = tc.btime;
        m_clock_white.time = tc.wtime;
    }

    auto makemove(const libataxx::Move &move, const int, const SearchSettings &clock) -> void {
        switch (clock.type) {
            case SearchSettings::Type::Time:
                m_clock_black.time = clock.btime;
                m_clock_white.time = clock.wtime;
                break;
            case SearchSettings::Type::Movetime:
                break;
            case SearchSettings::Type::Depth:
                break;
            case SearchSettings::Type::Nodes:
                break;
            default:
                break;
        }

        m_board.pos.makemove(move);

        if (m_board.pos.get_turn() == libataxx::Side::Black) {
            m_clock_black.ticking = true;
            m_clock_white.ticking = false;
        } else {
            m_clock_black.ticking = false;
            m_clock_white.ticking = true;
        }

        m_history.moves.emplace_back(static_cast<std::string>(move));
        m_material_bar.num_black = m_board.pos.get_black().count();
        m_material_bar.num_white = m_board.pos.get_white().count();
    }

    auto update_results(const Results &results) -> void {
        m_results = results;
    }

   private:
    std::string m_title = "Game";
    BoardComponent m_board;
    HistoryComponent m_history;
    ClockComponent m_clock_black;
    ClockComponent m_clock_white;
    MaterialBarComponent m_material_bar;
    Results m_results;
};

#endif
