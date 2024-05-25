#ifndef CUTEATAXX_TUI_COMPONENTS_MATERIAL_BAR_HPP
#define CUTEATAXX_TUI_COMPONENTS_MATERIAL_BAR_HPP

#include <ftxui/dom/elements.hpp>
#include <vector>

using namespace ftxui;

struct [[nodiscard]] MaterialBarComponent {
    int num_black = 0;
    int num_white = 0;

    [[nodiscard]] auto render() -> Element {
        std::vector<Element> bar;

        bar.emplace_back(bgcolor(Color::GrayDark, color(Color::Black, text(std::to_string(num_black) + " "))));

        for (int i = 0; i < 49; ++i) {
            const auto bg = i == 49 / 2 ? Color::Red : Color::GrayDark;

            if (i < num_black) {
                bar.emplace_back(bgcolor(bg, color(Color::Black, text("●"))));
            } else if (i < 49 - num_white) {
                bar.emplace_back(bgcolor(bg, color(Color::Grey11, text("-"))));
            } else {
                bar.emplace_back(bgcolor(bg, color(Color::White, text("●"))));
            }
        }

        bar.emplace_back(bgcolor(Color::GrayDark, color(Color::White, text(" " + std::to_string(num_white)))));

        return hbox({bar}) | center;
    }
};

#endif
