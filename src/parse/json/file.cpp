#include "file.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include "../openings.hpp"
#include "engines.hpp"
#include "pgn.hpp"
#include "timecontrol.hpp"

namespace parse {

namespace json {

std::pair<match::Settings, match::Engines> file(const std::string &path) {
    match::Settings settings;
    nlohmann::json j;

    // Get settings file
    {
        std::ifstream i(path);
        if (!i.is_open()) {
            throw std::invalid_argument("Could not open settings file " + path);
        }
        try {
            i >> j;
        } catch (nlohmann::json::exception &e) {
            throw e;
        } catch (...) {
            throw "Failure parsing .json";
        }
    }

    // Basic checks
    if (j.find("engines") == j.end()) {
        throw std::invalid_argument("Engines not found");
    }

    // Add number of games
    if (j.find("games") != j.end()) {
        settings.num_games = j.at("games");
    }

    // Add rating interval
    if (j.find("ratinginterval") != j.end()) {
        settings.ratinginterval = j.at("ratinginterval");
    }

    // Add colour1
    if (j.find("colour1") != j.end()) {
        settings.colour1 = j.at("colour1");
    }

    // Add colour2
    if (j.find("colour2") != j.end()) {
        settings.colour2 = j.at("colour2");
    }

    // Add repeat
    if (j.find("repeat") != j.end()) {
        settings.repeat = j.at("repeat");
    }

    // Add verbose
    if (j.find("verbose") != j.end()) {
        settings.verbose = j.at("verbose");
    }

    // Add openings path
    if (j.find("openings") != j.end()) {
        settings.openings_path = j.at("openings");
    }

    // Add timecontrol
    parse::json::timecontrol(j, settings);

    // Add pgn
    parse::json::pgn(j, settings);

    // Add engines
    auto engines = parse::json::engines(j);

    return {settings, engines};
}

}  // namespace json

}  // namespace parse
