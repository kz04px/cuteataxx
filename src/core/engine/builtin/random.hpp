#ifndef BUILTIN_RANDOM_HPP
#define BUILTIN_RANDOM_HPP

#include <functional>
#include <string>
#include "../engine.hpp"

class RandomBuiltin : public Engine {
   public:
    [[nodiscard]] RandomBuiltin(std::function<void(const std::string &msg)> send = {},
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
        const auto idx = rand() % moves.size();
        const auto move = moves.at(idx);
        return static_cast<std::string>(move);
    }

    [[nodiscard]] virtual auto is_running() -> bool override {
        return true;
    }

   private:
    libataxx::Position m_pos;
};

#endif
