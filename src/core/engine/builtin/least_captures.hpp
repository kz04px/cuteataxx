#ifndef BUILTIN_LEAST_CAPTURES_HPP
#define BUILTIN_LEAST_CAPTURES_HPP

#include <functional>
#include <string>
#include "../engine.hpp"

class LeastCapturesBuiltin : public Engine {
   public:
    [[nodiscard]] LeastCapturesBuiltin(std::function<void(const std::string &msg)> send = {},
                                       std::function<void(const std::string &msg)> recv = {})
        : Engine(send, recv) {
    }

    virtual auto init() -> void override {
    }

    virtual auto isready() -> void override {
    }

    virtual auto newgame() -> void override {
    }

    virtual auto quit() -> void override {
    }

    virtual auto stop() -> void override {
    }

    virtual auto position(const libataxx::Position &pos) -> void override {
        m_pos = pos;
    }

    virtual auto set_option(const std::string &, const std::string &) -> void override {
    }

    [[nodiscard]] virtual auto go(const SearchSettings &) -> std::string override {
        if (m_pos.is_gameover()) {
            return "0000";
        }

        const auto moves = m_pos.legal_moves();
        auto best_score = -1'000'000;
        auto best_move = libataxx::Move::nullmove();

        for (const auto &move : moves) {
            const auto score = -(m_pos.count_captures(move) + move.is_single());
            if (score > best_score) {
                best_score = score;
                best_move = move;
            }
        }

        return static_cast<std::string>(best_move);
    }

    [[nodiscard]] virtual auto is_running() -> bool override {
        return true;
    }

   private:
    libataxx::Position m_pos;
};

#endif
