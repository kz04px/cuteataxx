#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <functional>
#include <libataxx/position.hpp>
#include <string>
#include "settings.hpp"

class Engine {
   public:
    [[nodiscard]] Engine(std::function<void(const std::string &msg)> send = {},
                         std::function<void(const std::string &msg)> recv = {})
        : m_send(send), m_recv(recv) {
    }

    virtual ~Engine() {
    }

    [[nodiscard]] virtual auto go(const SearchSettings &settings) -> std::string = 0;

    virtual auto init() -> void = 0;

    virtual auto position(const libataxx::Position &pos) -> void = 0;

    virtual auto set_option(const std::string &name, const std::string &value) -> void = 0;

    virtual auto isready() -> void = 0;

    virtual auto newgame() -> void = 0;

   protected:
    [[nodiscard]] virtual auto is_running() -> bool = 0;

    virtual auto quit() -> void = 0;

    virtual auto stop() -> void = 0;

    std::function<void(const std::string &msg)> m_send;
    std::function<void(const std::string &msg)> m_recv;
};

#endif
