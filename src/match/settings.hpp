#ifndef MATCH_SETTINGS_HPP
#define MATCH_SETTINGS_HPP

#include <limits>
#include <map>
#include <ostream>
#include <string>
#include <vector>

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
    std::string name;
    std::string path;
    std::map<std::string, std::string> options;
};

struct Settings {
    int ratinginterval = 10;
    int concurrency = 1;
    int num_games = 100;
    int hash_size = 16;
    int timeout_buffer = 10;
    unsigned int maxfullmoves = std::numeric_limits<unsigned int>::max();
    bool debug = false;
    bool recover = false;
    bool verbose = false;
    bool repeat = true;
    bool shuffle = false;
    SearchSettings tc;
    std::string openings_path = "";
    std::vector<EngineSettings> engines;
    // pgn
    std::string pgn_path = "games.pgn";
    std::string pgn_event = "*";
    std::string colour1 = "white";
    std::string colour2 = "black";
    bool pgn_enabled = true;
    bool pgn_verbose = false;
    bool pgn_override = false;
};

struct GameSettings {
    std::string fen;
    EngineSettings engine1;
    EngineSettings engine2;
};

inline std::ostream &operator<<(std::ostream &os, const SearchSettings &ss) {
    switch (ss.type) {
        case SearchSettings::Type::Depth:
            os << "depth " << ss.ply;
            break;
        case SearchSettings::Type::Nodes:
            os << "nodes " << ss.nodes;
            break;
        case SearchSettings::Type::Movetime:
            os << "movetime " << ss.movetime << "ms";
            break;
        case SearchSettings::Type::Time:
            os << ss.btime << "+" << ss.binc << "ms";
            break;
        default:
            break;
    }
    return os;
}

#endif
