#include "settings.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace parse {

Settings settings(const std::string &path) {
    Settings settings;
    nlohmann::json json;

    // Get settings file
    {
        std::ifstream i(path);
        if (!i.is_open()) {
            throw std::invalid_argument("Could not open settings file " + path);
        }
        try {
            i >> json;
        } catch (nlohmann::json::exception &e) {
            throw e;
        } catch (...) {
            throw "Failure parsing .json";
        }
    }

    auto engine_iter = json.find("engines");

    // Basic checks
    if (engine_iter == json.end()) {
        throw std::invalid_argument("Engines not found");
    }

    // Default
    settings.tc.type = SearchSettings::Type::Movetime;
    settings.tc.movetime = 10;

    std::map<std::string, std::string> engine_options;

    for (const auto &[a, b] : json.items()) {
        if (a == "games") {
            settings.num_games = b.get<int>();
        } else if (a == "ratinginterval") {
            settings.ratinginterval = b.get<int>();
        } else if (a == "concurrency") {
            settings.concurrency = b.get<int>();
        } else if (a == "timeoutbuffer") {
            settings.timeout_buffer = b.get<int>();
        } else if (a == "maxfullmoves") {
            settings.maxfullmoves = b.get<int>();
        } else if (a == "colour1") {
            settings.colour1 = b.get<std::string>();
        } else if (a == "colour2") {
            settings.colour2 = b.get<std::string>();
        } else if (a == "debug") {
            settings.debug = b.get<bool>();
        } else if (a == "repeat") {
            settings.repeat = b.get<bool>();
        } else if (a == "verbose") {
            settings.verbose = b.get<bool>();
        } else if (a == "openings") {
            settings.openings_path = b.get<std::string>();
        } else if (a == "timecontrol") {
            for (const auto &[key, val] : b.items()) {
                if (key == "movetime") {
                    settings.tc.type = SearchSettings::Type::Movetime;
                    settings.tc.movetime = val.get<int>();
                } else if (key == "nodes") {
                    settings.tc.type = SearchSettings::Type::Nodes;
                    settings.tc.nodes = val.get<int>();
                } else if (key == "time") {
                    settings.tc.type = SearchSettings::Type::Time;
                    settings.tc.btime = val.get<int>();
                    settings.tc.wtime = val.get<int>();
                } else if (key == "increment" || key == "inc") {
                    settings.tc.type = SearchSettings::Type::Time;
                    settings.tc.binc = val.get<int>();
                    settings.tc.winc = val.get<int>();
                } else if (key == "depth") {
                    settings.tc.type = SearchSettings::Type::Depth;
                    settings.tc.ply = val.get<int>();
                }
            }
        } else if (a == "pgn") {
            for (const auto &[key, val] : b.items()) {
                if (key == "enabled") {
                    settings.pgn_enabled = val.get<bool>();
                } else if (key == "verbose") {
                    settings.pgn_verbose = val.get<bool>();
                } else if (key == "override") {
                    settings.pgn_override = val.get<bool>();
                } else if (key == "path") {
                    settings.pgn_path = val.get<std::string>();
                } else if (key == "event") {
                    settings.pgn_event = val.get<std::string>();
                }
            }
        } else if (a == "options") {
            for (const auto &[key, val] : b.items()) {
                engine_options[key] = val;
            }
        }
    }

    for (const auto &engine : *engine_iter) {
        EngineSettings details;

        details.id = settings.engines.size();
        details.options = engine_options;

        for (const auto &[a, b] : engine.items()) {
            if (a == "path") {
                details.path = b.get<std::string>();
            } else if (a == "name") {
                details.name = b.get<std::string>();
            } else if (a == "options") {
                for (const auto &[key, val] : b.items()) {
                    details.options[key] = val.get<std::string>();
                }
            }
        }

        // Use engine path as a backup name
        if (details.name.empty()) {
            details.name = details.path;
        }

        settings.engines.emplace_back(details);
    }

    return settings;
}

}  // namespace parse
