#ifndef CUTEATAXX_TUI_TABS_SETTINGS_HPP
#define CUTEATAXX_TUI_TABS_SETTINGS_HPP

#include <string>
#include <vector>
#include "base.hpp"

class [[nodiscard]] SettingsTab : public BaseTab {
   public:
    [[nodiscard]] auto render() -> Component {
        return Renderer(container, [this] {
            return vbox({
                text("Game settings"),
                container1->Render(),
                separator(),
                text("Theme settings"),
                container2->Render(),
            });
        });
    }

    const std::vector<std::string> theme_names = {
        "classic",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
        "other",
    };
    int theme_selected = 0;
    Component container1 = Container::Vertical({Checkbox("Show Eval", &show_eval)});
    Component container2 = Container::Vertical({Dropdown(&theme_names, &theme_selected)});
    Component container = Container::Vertical({
        container1,
        container2,
    });
    // Settings
    bool show_eval = false;
};

#endif
