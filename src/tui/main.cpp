#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
// Custom components
#include "components/board.hpp"
#include "components/clock.hpp"
// Core
#include "core/match/callbacks.hpp"
#include "core/match/run.hpp"
#include "core/parse/openings.hpp"
#include "core/parse/settings.hpp"
// Tabs
#include "tabs/about.hpp"
#include "tabs/game.hpp"
#include "tabs/results.hpp"
#include "tabs/schedule.hpp"
#include "tabs/settings.hpp"
// FTXUI
#include <ftxui/dom/table.hpp>
#include "ftxui/component/captured_mouse.hpp"      // for ftxui
#include "ftxui/component/component.hpp"           // for Radiobox, Renderer, Tab, Toggle, Vertical
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"                  // for Element, separator, operator|, vbox, border

using namespace ftxui;

struct Tabber {
    std::vector<std::string> tab_values;
    std::vector<Component> components;
    Component tab_toggle;
    Component tab_container;
    Component container;
    int tab_selected = 0;

    [[nodiscard]] auto render() -> Component {
        return Renderer(container, [&] {
            return vbox({
                       tab_toggle->Render(),
                       separator(),
                       tab_container->Render(),
                   }) |
                   border;
        });
    }

    auto add_tab(const std::string &name, const Component component) -> void {
        tab_values.emplace_back(name);
        components.emplace_back(component);

        tab_toggle = Toggle(&tab_values, &tab_selected);
        tab_container = Container::Tab(components, &tab_selected);
        container = Container::Vertical({tab_toggle, tab_container});
    }
};

[[nodiscard]] auto create_callbacks(const Settings &settings,
                                    GameTab &game_tab,
                                    ScreenInteractive &screen) -> Callbacks {
    auto callbacks = Callbacks{};

    callbacks.on_game_started = [&settings, &game_tab, &screen](
                                    const std::string &fen, const std::string &name1, const std::string &name2) {
        game_tab.new_game();
        game_tab.set_title(name1 + " vs " + name2);
        game_tab.set_position(fen);
        game_tab.set_clock(settings.tc);
        screen.PostEvent(Event::Custom);
    };

    callbacks.on_game_finished = [&settings](const libataxx::Result, const std::string &, const std::string &) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
    };

    callbacks.on_results_update = [&settings, &game_tab](const Results &results) {
        game_tab.update_results(results);
    };

    callbacks.on_move = [&game_tab, &screen](const libataxx::Move &move, const int ms) {
        game_tab.makemove(move, ms);
        screen.PostEvent(Event::Custom);
    };

    return callbacks;
}

auto main(const int argc, const char *const *const argv) -> int {
    if (argc < 2) {
        std::cerr << "Must provide path to settings file\n";
        return 1;
    }

    auto screen = ScreenInteractive::TerminalOutput();
    auto game_tab = GameTab();
    auto results_tab = ResultsTab();
    auto schedule_tab = ScheduleTab();
    auto settings_tab = SettingsTab();
    auto about_tab = AboutTab();

    const auto settings = parse::settings(argv[1]);
    const auto openings = parse::openings(settings.openings_path, settings.shuffle);
    const auto callbacks = create_callbacks(settings, game_tab, screen);

    auto tabber = Tabber();
    tabber.add_tab("Game", game_tab.render());
    tabber.add_tab("Results", results_tab.render());
    tabber.add_tab("Schedule", schedule_tab.render());
    tabber.add_tab("Settings", settings_tab.render());
    tabber.add_tab("About", about_tab.render());

    // Start match
    auto thread = std::thread([&settings, &openings, &callbacks]() {
        const auto results = run(settings, openings, callbacks);
    });

    screen.Loop(tabber.render());

    if (thread.joinable()) {
        thread.join();
    }

    return 0;
}
