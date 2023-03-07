#ifndef MATCH_SETTINGS_HPP
#define MATCH_SETTINGS_HPP

#include <optional>
#include <ostream>
#include <string>
#include <vector>
#include "engine/settings.hpp"
#include "tournament/types.hpp"

struct Settings {
    int ratinginterval = 10;
    int concurrency = 1;
    int num_games = 100;
    int hash_size = 16;
    int timeout_buffer = 10;
    bool debug = false;
    bool recover = false;
    bool verbose = false;
    bool repeat = true;
    bool shuffle = false;
    bool adjudicate = false;
    TournamentType tournament_type = TournamentType::RoundRobin;
    SearchSettings tc;
    std::string openings_path;
    std::vector<EngineSettings> engines;
    // Adjudication
    std::optional<int> adjudicate_gamelength;
    std::optional<int> adjudicate_material;
    std::optional<bool> adjudicate_easyfill;
    // pgn
    std::string pgn_path = "games.pgn";
    std::string pgn_event = "*";
    std::string colour1 = "black";
    std::string colour2 = "white";
    bool pgn_enabled = true;
    bool pgn_verbose = false;
    bool pgn_override = false;
    // SPRT
    bool sprt_enabled = false;
    bool sprt_autostop = false;
    float sprt_alpha = 0.05f;
    float sprt_beta = 0.05f;
    float sprt_elo0 = 0.0f;
    float sprt_elo1 = 5.0f;
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
