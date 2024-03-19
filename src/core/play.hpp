#ifndef PLAY_HPP
#define PLAY_HPP

#include <functional>
#include <libataxx/move.hpp>
#include <libataxx/position.hpp>
#include <memory>
#include <optional>
#include <vector>
#include "engine/settings.hpp"

enum class ResultReason : int
{
    Normal = 0,
    OutOfTime,
    MaterialImbalance,
    EasyFill,
    Gamelength,
    IllegalMove,
    EngineCrash,
    None,
};

class SearchSettings;
class Engine;

struct GameSettings {
    std::string fen;
    EngineSettings engine1;
    EngineSettings engine2;
};

struct AdjudicationSettings {
    std::optional<int> gamelength;
    std::optional<int> material;
    std::optional<bool> easyfill;
    int timeout_buffer = 0;
};

struct MoveThingy {
    libataxx::Move move = libataxx::Move::nomove();
    int movetime = 0;
};

struct GameThingy {
    libataxx::Result result = libataxx::Result::None;
    ResultReason reason = ResultReason::None;
    std::vector<MoveThingy> history;
    libataxx::Position startpos;
    libataxx::Position endpos;
};

[[nodiscard]] GameThingy play(
    const AdjudicationSettings &adjudication,
    const GameSettings &game,
    std::shared_ptr<Engine> engine1,
    std::shared_ptr<Engine> engine2,
    std::function<bool(GameThingy info, SearchSettings tc1, SearchSettings tc2)> on_new_move_callback =
        []([[maybe_unused]] GameThingy info, [[maybe_unused]] SearchSettings tc1, [[maybe_unused]] SearchSettings tc2) {
            return true;
        });

#endif
