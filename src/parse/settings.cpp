#include "settings.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace parse {

[[nodiscard]] Settings settings(const std::string &path) {
    Settings settings;
    nlohmann::ordered_json json;

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

    std::vector<std::pair<std::string, std::string>> engine_options;

    for (const auto &[a, b] : json.items()) {
        if (a == "games") {
            settings.num_games = b.get<int>();
        } else if (a == "ratinginterval") {
            settings.ratinginterval = b.get<int>();
        } else if (a == "concurrency") {
            settings.concurrency = b.get<int>();
        } else if (a == "timeoutbuffer") {
            settings.timeout_buffer = b.get<int>();
        } else if (a == "colour1") {
            settings.colour1 = b.get<std::string>();
        } else if (a == "colour2") {
            settings.colour2 = b.get<std::string>();
        } else if (a == "debug") {
            settings.debug = b.get<bool>();
        } else if (a == "verbose") {
            settings.verbose = b.get<bool>();
        } else if (a == "adjudicate") {
            for (const auto &[key, val] : b.items()) {
                if (key == "material") {
                    settings.adjudicate_material = val.get<int>();
                } else if (key == "easyfill") {
                    settings.adjudicate_easyfill = val.get<bool>();
                } else if (key == "gamelength") {
                    settings.adjudicate_gamelength = val.get<int>();
                }
            }
        } else if (a == "openings") {
            for (const auto &[key, val] : b.items()) {
                if (key == "path") {
                    settings.openings_path = val.get<std::string>();
                } else if (key == "repeat") {
                    settings.repeat = val.get<bool>();
                } else if (key == "shuffle") {
                    settings.shuffle = val.get<bool>();
                }
            }
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
                engine_options.emplace_back(key, val);
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
                    const auto iter =
                        std::find_if(details.options.begin(), details.options.end(), [key](const auto &obj) -> bool {
                            return obj.first == key;
                        });

                    // override
                    if (iter != details.options.end()) {
                        details.options.erase(iter);
                    }

                    details.options.emplace_back(key, val.get<std::string>());
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
