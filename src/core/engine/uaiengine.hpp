#ifndef UAI_ENGINE_PROCESS_HPP
#define UAI_ENGINE_PROCESS_HPP

#include <libataxx/position.hpp>
#include <string>
#include <string_view>
#include <utils.hpp>
#include "process.hpp"

class UAIEngine final : public ProcessEngine {
   public:
    [[nodiscard]] UAIEngine(const std::string &path,
                            const std::string &arguments,
                            std::function<void(const std::string &msg)> send = {},
                            std::function<void(const std::string &msg)> recv = {})
        : ProcessEngine(path, arguments, send, recv) {
    }

    ~UAIEngine() {
        if (is_running()) {
            send("stop");
            send("quit");
        }
    }

    virtual auto init() -> void override {
        send("uai");
        wait_for("uaiok");
    }

    virtual void isready() override {
        send("isready");
        wait_for("readyok");
    }

    virtual void newgame() override {
        send("uainewgame");
    }

    virtual void quit() override {
        send("quit");
    }

    virtual void stop() override {
        send("stop");
    }

    virtual auto position(const libataxx::Position &pos) -> void override {
        send("position fen " + pos.get_fen());
    }

    virtual auto set_option(const std::string &name, const std::string &value) -> void override {
        send("setoption name " + name + " value " + value);
    }

    [[nodiscard]] virtual auto go(const SearchSettings &settings) -> std::string override {
        switch (settings.type) {
            case SearchSettings::Type::Time: {
                auto str = std::string();
                str += "go";
                str += " btime " + std::to_string(settings.btime);
                str += " wtime " + std::to_string(settings.wtime);
                str += " binc " + std::to_string(settings.binc);
                str += " winc " + std::to_string(settings.winc);
                send(str);
                break;
            }
            case SearchSettings::Type::Movetime:
                send("go movetime " + std::to_string(settings.movetime));
                break;
            case SearchSettings::Type::Depth:
                send("go depth " + std::to_string(settings.ply));
                break;
            case SearchSettings::Type::Nodes:
                send("go nodes " + std::to_string(settings.nodes));
                break;
            default:
                return {};
        }

        auto movestr = std::string("0000");

        wait_for([&movestr](const std::string_view msg) {
            const auto parts = utils::split(msg);
            auto got_bestmove = false;

            for (std::size_t i = 0; i < parts.size(); i += 2) {
                if (parts[i] == "bestmove") {
                    movestr = parts[i + 1];
                    got_bestmove = true;
                }
            }

            return got_bestmove;
        });

        return movestr;
    }

   private:
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
};

#endif
