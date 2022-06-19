#ifndef UCI_ENGINE_PROCESS_HPP
#define UCI_ENGINE_PROCESS_HPP

#include <libataxx/position.hpp>
#include <string>
#include <string_view>
#include <utils.hpp>
#include "engine.hpp"
#include "match/settings.hpp"

[[nodiscard]] inline auto fen_to_ucifen(const std::string &fen) noexcept -> std::string {
    auto nfen = fen;
    std::size_t idx = 0;

    while (idx < nfen.size() && nfen[idx] != ' ') {
        if (nfen[idx] == 'x') {
            nfen[idx] = 'P';
        } else if (nfen[idx] == 'o') {
            nfen[idx] = 'p';
        } else if (nfen[idx] == ' ') {
            break;
        }

        idx++;
    }

    idx++;

    if (idx < nfen.size()) {
        if (nfen[idx] == 'x') {
            nfen[idx] = 'w';
        } else if (nfen[idx] == 'o') {
            nfen[idx] = 'b';
        }
    }

    return nfen;
}

class UCIEngine : public Engine {
   public:
    [[nodiscard]] UCIEngine(const std::string &path,
                            std::function<void(const std::string &msg)> send = {},
                            std::function<void(const std::string &msg)> recv = {})
        : Engine(path, send, recv) {
    }

    ~UCIEngine() {
        if (is_running()) {
            send("stop");
            send("quit");
        }
    }

    virtual auto init() -> void override {
        send("uci");
        wait_for("uciok");
    }

    virtual void isready() override {
        send("isready");
        wait_for("readyok");
    }

    virtual void newgame() override {
        send("ucinewgame");
    }

    virtual void quit() override {
        send("quit");
    }

    virtual void stop() override {
        send("stop");
    }

    virtual auto position(const libataxx::Position &pos) -> void override {
        const auto nfen = fen_to_ucifen(pos.get_fen());
        send("position fen " + nfen);
    }

    virtual auto set_option(const std::string &name, const std::string &value) -> void override {
        send("setoption name " + name + " value " + value);
    }

    [[nodiscard]] virtual auto go(const SearchSettings &settings) -> std::string override {
        switch (settings.type) {
            case SearchSettings::Type::Time: {
                auto str = std::string();
                str += "go";
                str += " wtime " + std::to_string(settings.btime);
                str += " btime " + std::to_string(settings.wtime);
                str += " winc " + std::to_string(settings.binc);
                str += " binc " + std::to_string(settings.winc);
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

        // Passing moves are encoded as [sq][sq] instead of 0000
        if (movestr[0] == movestr[2] && movestr[1] == movestr[3]) {
            return "0000";
        }

        // Turn long form dropping moves into short form
        // Example: X@g2 ---> g2
        if (movestr[1] == '@') {
            return movestr.substr(2, 2);
        }

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
