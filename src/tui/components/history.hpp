#ifndef CUTEATAXX_TUI_COMPONENTS_HISTORY_HPP
#define CUTEATAXX_TUI_COMPONENTS_HISTORY_HPP

#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <string>
#include <vector>

using namespace ftxui;

struct [[nodiscard]] HistoryComponent {
    std::string title = "Moves";
    std::vector<std::string> moves;
    bool white_first = false;

    [[nodiscard]] auto render() -> Element {
        const std::size_t max_rows = 27;
        const std::size_t end_row = (moves.size() + white_first + 1) / 2;
        const std::size_t start_row = end_row > max_rows ? end_row - max_rows : 0;

        // Create elements
        auto elements = std::vector<std::vector<Element>>({});
        for (std::size_t row = start_row; row < end_row; ++row) {
            const auto idx = 2 * row + 1 - white_first;
            const auto movenum = text(std::to_string(row + 1));
            const auto move1 = flex_grow(idx > 0 ? text(moves[idx - 1]) : text("..."));
            const auto move2 = flex_grow(idx < moves.size() ? text(moves[idx]) : filler());
            const auto line = std::vector<Element>({{
                movenum,
                separator(),
                move1,
                separator(),
                move2,
            }});

            elements.emplace_back(line);
        }

        return Table(elements).Render() | yflex_grow | border;
    }
};

#endif
