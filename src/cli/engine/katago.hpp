#ifndef KATAGO_ENGINE_PROCESS_HPP
#define KATAGO_ENGINE_PROCESS_HPP

#include <libataxx/position.hpp>
#include <string>
#include <string_view>
#include <utils.hpp>
#include "process.hpp"

class KataGo : public ProcessEngine {
   public:
    [[nodiscard]] KataGo(const std::string &path,
                         const std::string &arguments,
                         std::function<void(const std::string &msg)> send = {},
                         std::function<void(const std::string &msg)> recv = {})
        : ProcessEngine(path, arguments, send, recv) {
    }

    ~KataGo() {
        if (is_running()) {
            send("quit");
            wait_for_first("=");
        }
    }

    virtual auto init() -> void override {
        send("boardsize 7");
        wait_for_first("=");
        send("komi 0");
        wait_for_first("=");
    }

    virtual void isready() override {
    }

    virtual void newgame() override {
        send("clear_cache");
        wait_for_first("=");
        send("clear_board");
        wait_for_first("=");
    }

    virtual void quit() override {
        send("quit");
        wait_for_first("=");
    }

    virtual void stop() override {
    }

    virtual auto position(const libataxx::Position &pos) -> void override {
        m_is_black = pos.turn() == libataxx::Side::Black;

        std::string command = "set_position";

        for (const auto sq : pos.black()) {
            command += " black " + static_cast<std::string>(sq);
        }

        for (const auto sq : pos.white()) {
            command += " white " + static_cast<std::string>(sq);
        }

        send(command);
        wait_for_first("=");
    }

    virtual auto set_option(const std::string &name, const std::string &value) -> void override {
    }

    [[nodiscard]] virtual auto go(const SearchSettings &settings) -> std::string override {
        switch (settings.type) {
            case SearchSettings::Type::Time: {
                const auto time = static_cast<float>(m_is_black ? settings.btime : settings.wtime) / 1000;
                const auto inc = static_cast<float>(m_is_black ? settings.binc : settings.winc) / 1000;
                send("kata-time_settings fischer " + std::to_string(time) + " " + std::to_string(inc));
                wait_for_first("=");
                break;
            }
            case SearchSettings::Type::Movetime: {
                const auto seconds = static_cast<float>(settings.movetime) / 1000;
                send("time_settings 0 " + std::to_string(seconds) + " 1");
                wait_for_first("=");
                break;
            }
            case SearchSettings::Type::Depth:
                break;
            case SearchSettings::Type::Nodes:
                break;
            default:
                return {};
        }

        send("genmove");
        auto fromstr = std::string("0000");
        wait_for([&fromstr](const std::string_view msg) {
            const auto parts = utils::split(msg);
            auto got_bestmove = false;
            for (std::size_t i = 0; i + 1 < parts.size(); i += 2) {
                if (parts[i] == "=") {
                    fromstr = parts[i + 1];
                    got_bestmove = true;
                }
            }
            return got_bestmove;
        });

        send("genmove");
        auto tostr = std::string("0000");
        wait_for([&tostr](const std::string_view msg) {
            const auto parts = utils::split(msg);
            auto got_bestmove = false;
            for (std::size_t i = 0; i + 1 < parts.size(); i += 2) {
                if (parts[i] == "=") {
                    tostr = parts[i + 1];
                    got_bestmove = true;
                }
            }
            return got_bestmove;
        });

        if (fromstr == "pass") {
            return tostr;
        }

        if (tostr == "pass") {
            return "0000";
        }

        return fromstr + tostr;
    }

   private:
    auto wait_for_first(const std::string &msg) -> void {
        while (is_running()) {
            const auto line = get_output();
            if (line.empty()) {
                continue;
            }

            const auto parts = utils::split(msg);
            if (!parts.empty() && parts[0] == msg) {
                break;
            }
        }
    }

    auto wait_for(const std::string &msg) -> void {
        while (is_running()) {
            const auto line = get_output();
            if (line == msg) {
                break;
            }
        }
    }

    auto wait_for(const std::function<bool(const std::string_view msg)> func) -> void {
        auto exit = false;
        while (is_running() && !exit) {
            const auto line = get_output();
            exit = func(line);
        }
    }

    bool m_is_black = true;
};

#endif
