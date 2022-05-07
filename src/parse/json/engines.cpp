#include "engines.hpp"

namespace parse::json {

std::vector<EngineSettings> engines(const nlohmann::json &j) {
    int count = 0;
    std::vector<EngineSettings> engines;
    for (const auto &engine : j.at("engines")) {
        EngineSettings details;
        // Necessary -- path
        if (engine.find("path") == engine.end()) {
            continue;
        }

        // Engine ID
        details.id = count;

        details.path = engine.at("path");

        // Use engine path as a backup name
        if (engine.find("name") == engine.end()) {
            details.name = engine.at("path");
        } else {
            details.name = engine.at("name");
        }

        // Engine options
        if (engine.find("options") != engine.end()) {
            for (const auto &[key, val] : engine.at("options").items()) {
                details.options[key] = val;
            }
        }

        engines.emplace_back(details);
        count++;
    }
    return engines;
}

}  // namespace parse::json
