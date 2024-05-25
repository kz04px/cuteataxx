#ifndef CUTEATAXX_TUI_TABS_SCHEDULE_HPP
#define CUTEATAXX_TUI_TABS_SCHEDULE_HPP

#include "base.hpp"

class [[nodiscard]] ScheduleTab : public BaseTab {
   public:
    [[nodiscard]] auto render() -> Component {
        return Renderer([this] {
            return text("Schedule");
        });
    }
};

#endif
