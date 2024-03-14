#ifndef MATCH_SETTINGS_HPP
#define MATCH_SETTINGS_HPP

#include <optional>
#include <ostream>
#include <string>
#include <vector>
#include "../engine/settings.hpp"
#include "../pgn.hpp"
#include "../tournament/types.hpp"

struct SPRTSettings {
    bool enabled = false;
    bool autostop = false;
    float alpha = 0.05f;
    float beta = 0.05f;
    float elo0 = 0.0f;
    float elo1 = 5.0f;
};

struct Settings {
    int ratinginterval = 10;
    int concurrency = 1;
    int num_games = 100;
    bool debug = false;
    bool recover = false;
    bool verbose = false;
    bool repeat = true;
    bool shuffle = false;
    bool print_early = true;
    TournamentType tournament_type = TournamentType::RoundRobin;
    std::string openings_path;
    std::vector<EngineSettings> engines;
    SearchSettings tc;
    AdjudicationSettings adjudication;
    PGNSettings pgn;
    SPRTSettings sprt;
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
