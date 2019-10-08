#include "engines.hpp"

namespace parse {

namespace json {

match::Engines engines(const nlohmann::json &j) {
    match::Engines engines;
    for (const auto &engine : j.at("engines")) {
        match::Details details;
        // Necessary -- path
        if (engine.find("path") == engine.end()) {
            continue;
        }

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

        engines[details.name] = details;
    }
    return engines;
}

}  // namespace json

}  // namespace parse
