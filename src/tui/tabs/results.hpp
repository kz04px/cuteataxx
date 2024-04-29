#ifndef CUTEATAXX_TUI_TABS_RESULTS_HPP
#define CUTEATAXX_TUI_TABS_RESULTS_HPP

#include <string>
#include <vector>
#include "base.hpp"
// FTXUI
#include <ftxui/dom/table.hpp>
#include "ftxui/dom/elements.hpp"

class [[nodiscard]] ResultsTab : public BaseTab {
   public:
    [[nodiscard]] auto render() -> Component {
        return Renderer([this] {
            std::vector<std::vector<Element>> elements = {
                {text("Rank"), text("Engine"), text("Wins"), text("Losses"), text("Draws")}};

            for (const auto &result : match_results.results) {
                std::vector<Element> row;
                row.emplace_back(text(std::to_string(elements.size())));
                row.emplace_back(text(result.name));
                row.emplace_back(text(std::to_string(result.wins)));
                row.emplace_back(text(std::to_string(result.losses)));
                row.emplace_back(text(std::to_string(result.draws)));
                elements.emplace_back(row);
            }

            auto table = Table(elements);
            table.SelectAll().Border(LIGHT);

            // Add border around the first column
            table.SelectColumn(0).Border(LIGHT);
            table.SelectColumn(1).Border(LIGHT);
            table.SelectColumn(2).Border(LIGHT);
            table.SelectColumn(3).Border(LIGHT);
            table.SelectColumn(4).Border(LIGHT);

            // Make first row bold with a border
            table.SelectRow(0).Decorate(bold);
            table.SelectRow(0).Border(LIGHT);

            // Alternate in between 2 colors
            auto content = table.SelectRows(1, -1);
            content.DecorateCellsAlternateRow(color(Color::Cyan), 2, 1);
            content.DecorateCellsAlternateRow(color(Color::White), 2, 2);

            return table.Render();
        });
    }

   private:
    struct Result {
        std::string name;
        int wins = 0;
        int losses = 0;
        int draws = 0;
        int crashes = 0;
        int timeouts = 0;
    };

    struct MatchResults {
        std::vector<Result> results;
        int games = 0;
        int black_wins = 0;
        int white_wins = 0;
        int draws = 0;
    };

    MatchResults match_results = MatchResults{
        {
            {"Autaxx", 999, 0, 0},
            {"Noobling", 0, 999, 0},
            {"Test1", 0, 0, 0},
            {"Test2", 0, 0, 0},
            {"Test3", 0, 0, 0},
            {"Test4", 0, 0, 0},
            {"Test5", 0, 0, 0},
            {"Test6", 0, 0, 0},
        },
        1998,
    };
};

#endif
