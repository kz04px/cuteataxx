#ifndef ENGINE_PROCESS_HPP
#define ENGINE_PROCESS_HPP

#include <boost/process.hpp>
#include <functional>
#include <libataxx/move.hpp>
#include <libataxx/position.hpp>
#include <string>
#include <string_view>
#include <utils.hpp>

struct SearchSettings {
    enum class Type : int
    {
        Time = 0,
        Movetime,
        Depth,
        Nodes,
    };

    [[nodiscard]] static auto as_time(const int bt, const int wt, const int bi, const int wi) -> SearchSettings {
        SearchSettings s;
        s.type = Type::Time;
        s.btime = bt;
        s.wtime = wt;
        s.binc = bi;
        s.winc = wi;
        return s;
    }

    [[nodiscard]] static auto as_movetime(const int time) -> SearchSettings {
        SearchSettings s;
        s.type = Type::Movetime;
        s.movetime = time;
        return s;
    }

    [[nodiscard]] static auto as_depth(const int p) -> SearchSettings {
        SearchSettings s;
        s.type = Type::Depth;
        s.ply = p;
        return s;
    }

    [[nodiscard]] static auto as_nodes(const int n) -> SearchSettings {
        SearchSettings s;
        s.type = Type::Nodes;
        s.nodes = n;
        return s;
    }

    Type type = Type::Depth;
    int btime = 0;
    int wtime = 0;
    int binc = 0;
    int winc = 0;
    int movestogo = 0;
    int movetime = 0;
    int ply = 0;
    int nodes = 0;
};

class UAIEngine {
   private:
    auto send(const std::string &msg) -> void {
        m_in << msg << std::endl;
    }

    auto wait_for(const std::string &msg) {
        std::string line;
        while (is_running()) {
            std::getline(m_out, line);
            if (line == msg) {
                break;
            }
        }
    }

    auto wait_for(const std::function<bool(const std::string_view msg)> func) {
        std::string line;
        auto exit = false;
        while (is_running() && !exit) {
            std::getline(m_out, line);
            exit = func(line);
        }
    }

    boost::process::opstream m_in;
    boost::process::ipstream m_out;
    boost::process::child m_child;

   public:
    [[nodiscard]] UAIEngine(const std::string &path)
        : m_child(path, boost::process::std_out > m_out, boost::process::std_in < m_in) {
    }

    [[nodiscard]] auto is_running() -> bool {
        return m_child.running();
    }

    void uai() {
        send("uai");
        wait_for("uaiok");
    }

    void isready() {
        send("isready");
        wait_for("readyok");
    }

    void uainewgame() {
        send("uainewgame");
    }

    void quit() {
        send("quit");
    }

    void stop() {
        send("stop");
    }

    auto position(const libataxx::Position &pos) -> void {
        send("position fen " + pos.get_fen());
    }

    auto set_option(const std::string &name, const std::string &value) -> void {
        send("setoption name " + name + " value " + value);
    }

    [[nodiscard]] auto go(const SearchSettings &settings) -> libataxx::Move {
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

        return libataxx::Move::from_uai(movestr);
    }
};

#endif
