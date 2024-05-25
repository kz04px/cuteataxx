#ifndef CUTEATAXX_CORE_CALLBACKS_HPP
#define CUTEATAXX_CORE_CALLBACKS_HPP

#include <functional>
#include <string>
#include "results.hpp"

struct Callbacks {
    std::function<void(const std::string &)> on_engine_start = [](const auto) {
    };
    std::function<void(const int, const std::string &, const std::string &)> on_game_started =
        [](const auto, const auto, const auto) {
        };
    std::function<void(const int, const std::string &, const std::string &)> on_game_finished =
        [](const auto, const auto, const auto) {
        };
    std::function<void(const Results &)> on_results_update = [](const auto) {
    };
    std::function<void(const std::string &)> on_info_send = [](const auto) {
    };
    std::function<void(const std::string &)> on_info_recv = [](const auto) {
    };
};

#endif
