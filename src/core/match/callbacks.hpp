#ifndef CUTEATAXX_CORE_CALLBACKS_HPP
#define CUTEATAXX_CORE_CALLBACKS_HPP

#include <functional>
#include <libataxx/move.hpp>
#include <string>
#include "results.hpp"

struct Callbacks {
    std::function<void(const std::string &)> on_engine_start;
    std::function<void(const int, const std::string &, const std::string &, const std::string &)> on_game_started;
    std::function<void(const int, const std::string &, const std::string &)> on_game_finished;
    std::function<void(const Results &)> on_results_update;
    std::function<void(const std::string &)> on_info_send;
    std::function<void(const std::string &)> on_info_recv;
    std::function<void(const libataxx::Move &, const int)> on_move;
};

#endif
