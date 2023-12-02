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
            throw std::logic_error("Failure parsing .json");
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
        } else if (a == "colour1") {
            settings.pgn.colour1 = b.get<std::string>();
        } else if (a == "colour2") {
            settings.pgn.colour2 = b.get<std::string>();
        } else if (a == "debug") {
            settings.debug = b.get<bool>();
        } else if (a == "verbose") {
            settings.verbose = b.get<bool>();
        } else if (a == "print_early") {
            settings.print_early = b.get<bool>();
        } else if (a == "tournament") {
            const auto tournament_type = b.get<std::string>();
            if (tournament_type == "roundrobin") {
                settings.tournament_type = TournamentType::RoundRobin;
            } else if (tournament_type == "gauntlet") {
                settings.tournament_type = TournamentType::Gauntlet;
            }
        } else if (a == "adjudicate") {
            for (const auto &[key, val] : b.items()) {
                if (key == "material") {
                    settings.adjudication.material = val.get<int>();
                } else if (key == "easyfill") {
                    settings.adjudication.easyfill = val.get<bool>();
                } else if (key == "gamelength") {
                    settings.adjudication.gamelength = val.get<int>();
                } else if (a == "timeout_buffer") {
                    settings.adjudication.timeout_buffer = b.get<int>();
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
                    settings.pgn.enabled = val.get<bool>();
                } else if (key == "verbose") {
                    settings.pgn.verbose = val.get<bool>();
                } else if (key == "override") {
                    settings.pgn.override = val.get<bool>();
                } else if (key == "path") {
                    settings.pgn.path = val.get<std::string>();
                } else if (key == "event") {
                    settings.pgn.event = val.get<std::string>();
                }
            }
        } else if (a == "sprt") {
            for (const auto &[key, val] : b.items()) {
                if (key == "enabled") {
                    settings.sprt.enabled = val.get<bool>();
                } else if (key == "autostop") {
                    settings.sprt.autostop = val.get<bool>();
                } else if (key == "confidence") {
                    settings.sprt.alpha = 1.0f - val.get<float>();
                    settings.sprt.beta = 1.0f - val.get<float>();
                } else if (key == "alpha") {
                    settings.sprt.alpha = val.get<float>();
                } else if (key == "beta") {
                    settings.sprt.beta = val.get<float>();
                } else if (key == "elo0") {
                    settings.sprt.elo0 = val.get<float>();
                } else if (key == "elo1") {
                    settings.sprt.elo1 = val.get<float>();
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
        details.tc = settings.tc;

        for (const auto &[a, b] : engine.items()) {
            if (a == "path") {
                details.path = b.get<std::string>();
            } else if (a == "protocol") {
                const auto proto = b.get<std::string>();
                if (proto == "UAI" || proto == "uai") {
                    details.proto = EngineProtocol::UAI;
                } else if (proto == "FSF" || proto == "fsf") {
                    details.proto = EngineProtocol::FSF;
                } else if (proto == "KATAGO" || proto == "KataGo" || proto == "katago") {
                    details.proto = EngineProtocol::KataGo;
                }
            } else if (a == "name") {
                details.name = b.get<std::string>();
            } else if (a == "builtin") {
                details.builtin = b.get<std::string>();
            } else if (a == "arguments") {
                details.arguments = b.get<std::string>();
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
            } else if (a == "timecontrol") {
                for (const auto &[key, val] : b.items()) {
                    if (key == "movetime") {
                        details.tc.type = SearchSettings::Type::Movetime;
                        details.tc.movetime = val.get<int>();
                    } else if (key == "nodes") {
                        details.tc.type = SearchSettings::Type::Nodes;
                        details.tc.nodes = val.get<int>();
                    } else if (key == "time") {
                        details.tc.type = SearchSettings::Type::Time;
                        details.tc.btime = val.get<int>();
                        details.tc.wtime = val.get<int>();
                    } else if (key == "increment" || key == "inc") {
                        details.tc.type = SearchSettings::Type::Time;
                        details.tc.binc = val.get<int>();
                        details.tc.winc = val.get<int>();
                    } else if (key == "depth") {
                        details.tc.type = SearchSettings::Type::Depth;
                        details.tc.ply = val.get<int>();
                    }
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
