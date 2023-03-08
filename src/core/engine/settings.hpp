#ifndef ENGINE_SETTINGS_HPP
#define ENGINE_SETTINGS_HPP

#include <string>
#include <vector>

enum class EngineProtocol : int
{
    UAI,
    FSF,
    KataGo,
    Unknown,
};

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

struct EngineSettings {
    int id;
    EngineProtocol proto = EngineProtocol::Unknown;
    std::string name;
    std::string builtin;
    std::string path;
    std::string arguments;
    SearchSettings tc;
    std::vector<std::pair<std::string, std::string>> options;
};

#endif
