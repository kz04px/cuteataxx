#ifndef CUTEATAXX_TUI_COMPONENTS_BOARD_HPP
#define CUTEATAXX_TUI_COMPONENTS_BOARD_HPP

#include <libataxx/position.hpp>
#include <string>
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"

using namespace ftxui;

// Round
// const std::string row1 = "         ";
// const std::string row2 = "   ◢█◣   ";
// const std::string row3 = "   ███   ";
// const std::string row4 = "   ◥█◤   ";
// const std::string row5 = "         ";

// Janky
const std::string row1 = "         ";
const std::string row2 = "   ◢█◣   ";
const std::string row3 = "  █████  ";
const std::string row4 = "   ◥█◤   ";
const std::string row5 = "         ";

// Demon
// const std::string row1 = "         ";
// const std::string row2 = "  ◣ █ ◢  ";
// const std::string row3 = "  █████  ";
// const std::string row4 = "   ◥█◤   ";
// const std::string row5 = "         ";

// Torch
// const std::string row1 = "    ◢    ";
// const std::string row2 = "   ◢◤◢   ";
// const std::string row3 = "   ◥█◤   ";
// const std::string row4 = "  ◥███◤  ";
// const std::string row5 = "    █    ";

// Bishop
// const std::string row1 = "    ◢    ";
// const std::string row2 = "   ◢◤◢   ";
// const std::string row3 = "   ◥█◤   ";
// const std::string row4 = "    █    ";
// const std::string row5 = "   ███   ";

// Fox
// const std::string row1 = "         ";
// const std::string row2 = "  ◣   ◢  ";
// const std::string row3 = "  █████  ";
// const std::string row4 = "   ◥█◤   ";
// const std::string row5 = "         ";

// UFO
// const std::string row1 = "         ";
// const std::string row2 = "   ◢█◣   ";
// const std::string row3 = "  █████  ";
// const std::string row4 = "  ◢◤ ◥◣  ";
// const std::string row5 = "         ";

[[nodiscard]] auto create_grid(const Color background, const libataxx::Piece piece) -> Element {
    switch (piece) {
        case libataxx::Piece::Black:
            return gridbox({
                {bgcolor(background, color(Color::Black, text(row1)))},
                {bgcolor(background, color(Color::Black, text(row2)))},
                {bgcolor(background, color(Color::Black, text(row3)))},
                {bgcolor(background, color(Color::Black, text(row4)))},
                {bgcolor(background, color(Color::Black, text(row5)))},
            });
        case libataxx::Piece::White:
            return gridbox({
                {bgcolor(background, color(Color::White, text(row1)))},
                {bgcolor(background, color(Color::White, text(row2)))},
                {bgcolor(background, color(Color::White, text(row3)))},
                {bgcolor(background, color(Color::White, text(row4)))},
                {bgcolor(background, color(Color::White, text(row5)))},
            });
        case libataxx::Piece::Gap:
            return gridbox({
                {bgcolor(Color::GrayDark, text("         "))},
                {bgcolor(Color::GrayDark, text("         "))},
                {bgcolor(Color::GrayDark, text("         "))},
                {bgcolor(Color::GrayDark, text("         "))},
                {bgcolor(Color::GrayDark, text("         "))},
            });
        case libataxx::Piece::Empty:
            return gridbox({
                {bgcolor(background, text("         "))},
                {bgcolor(background, text("         "))},
                {bgcolor(background, text("         "))},
                {bgcolor(background, text("         "))},
                {bgcolor(background, text("         "))},
            });
        default:
            return gridbox({
                {bgcolor(background, text("  ◢███◣  "))},
                {bgcolor(background, text("  █   █  "))},
                {bgcolor(background, text("    ◢█◤  "))},
                {bgcolor(background, text("         "))},
                {bgcolor(background, text("    █    "))},
            });
    }
}

struct [[nodiscard]] BoardComponent {
    libataxx::Position pos;
    std::array<Element, 49> grid{};

    BoardComponent() {
        update();
    }

    auto update() -> void {
        const auto bg_light = Color::CyanLight;
        const auto bg_dark = Color::Cyan;

        for (int y = 0; y < 7; ++y) {
            for (int x = 0; x < 7; ++x) {
                const auto idx = 7 * y + x;
                const auto bg = (x % 2 + y % 2) % 2 ? bg_dark : bg_light;
                const auto sq = libataxx::Square(x, y);
                const auto piece = pos.get(sq);
                grid.at(idx) = create_grid(bg, piece);
            }
        }
    }

    [[nodiscard]] auto render() -> Element {
        update();
        return gridbox({
            {grid[42], grid[43], grid[44], grid[45], grid[46], grid[47], grid[48]},
            {grid[35], grid[36], grid[37], grid[38], grid[39], grid[40], grid[41]},
            {grid[28], grid[29], grid[30], grid[31], grid[32], grid[33], grid[34]},
            {grid[21], grid[22], grid[23], grid[24], grid[25], grid[26], grid[27]},
            {grid[14], grid[15], grid[16], grid[17], grid[18], grid[19], grid[20]},
            {grid[7], grid[8], grid[9], grid[10], grid[11], grid[12], grid[13]},
            {grid[0], grid[1], grid[2], grid[3], grid[4], grid[5], grid[6]},
        });
    }
};

#endif
