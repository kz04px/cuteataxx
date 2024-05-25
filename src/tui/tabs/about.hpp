#ifndef CUTEATAXX_TUI_TABS_ABOUT_HPP
#define CUTEATAXX_TUI_TABS_ABOUT_HPP

#include "base.hpp"

class [[nodiscard]] AboutTab : public BaseTab {
   public:
    [[nodiscard]] auto render() -> Component {
        return Renderer([this] {
            return vbox({
                text("CuteAtaxx TUI v0.1.0"),
                text("Made by kz04px"),
                text("https://github.com/kz04px/cuteataxx"),
            });
        });
    }
};

#endif
