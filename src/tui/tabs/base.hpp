#ifndef CUTEATAXX_TUI_TABS_BASE_HPP
#define CUTEATAXX_TUI_TABS_BASE_HPP

#include "ftxui/component/component.hpp"

class [[nodiscard]] BaseTab {
   public:
    [[nodiscard]] virtual auto render() -> Component = 0;

   private:
};

#endif
