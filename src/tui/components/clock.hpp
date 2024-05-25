#ifndef CUTEATAXX_TUI_COMPONENTS_CLOCK_HPP
#define CUTEATAXX_TUI_COMPONENTS_CLOCK_HPP

#include <chrono>
#include <ftxui/dom/elements.hpp>
#include <iomanip>
#include <sstream>
#include <string>

using namespace ftxui;

[[nodiscard]]
auto format_seconds(const int ms) -> std::string {
    const auto hh_mm_ss = std::chrono::hh_mm_ss<std::chrono::milliseconds>(std::chrono::milliseconds(ms));
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hh_mm_ss.hours().count() << "h ";
    ss << std::setfill('0') << std::setw(2) << hh_mm_ss.minutes().count() << "m ";
    ss << std::setfill('0') << std::setw(2) << hh_mm_ss.seconds().count() << "s";
    return ss.str();
}

struct [[nodiscard]] ClockComponent {
    std::string title = "Clock";
    int time = 0;
    bool ticking = false;
    Color colour = Color::White;

    [[nodiscard]] auto render() -> Element {
        const auto msg = ticking ? hbox({
                                       color(colour, text("●●● ")),
                                       text(title) | center | flex,
                                       color(colour, text(" ●●●")),
                                   })
                                 : hbox({
                                       text(title) | center | flex,
                                   });
        return vbox({
                   msg,
                   text(format_seconds(time)) | center,
               }) |
               border;
    }
};

#endif
