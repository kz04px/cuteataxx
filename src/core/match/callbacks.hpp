#ifndef CUTEATAXX_CORE_CALLBACKS_HPP
#define CUTEATAXX_CORE_CALLBACKS_HPP

#include <functional>
#include <libataxx/move.hpp>
#include <libataxx/position.hpp>
#include <string>
#include "results.hpp"

class SearchSettings;

struct Callbacks {
    std::function<void(const std::string &)> on_engine_start = [](const auto) {
    };
    std::function<void(const std::size_t, const std::string &, const std::string &, const std::string &)>
        on_game_started = [](const auto, const auto, const auto, const auto) {
        };
    std::function<void(const std::size_t, const libataxx::Result, const std::string &, const std::string &)>
        on_game_finished = [](const auto, const auto, const auto, const auto) {
        };
    std::function<void(const Results &)> on_results_update = [](const auto) {
    };
    std::function<void(const std::string &)> on_info_send = [](const auto) {
    };
    std::function<void(const std::string &)> on_info_recv = [](const auto) {
    };
    std::function<void(const libataxx::Move &, const int, const SearchSettings &)> on_move =
        [](const auto, const auto, const auto &) {
        };
};

#endif
