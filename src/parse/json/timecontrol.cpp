#include "timecontrol.hpp"
#include <iostream>

namespace parse::json {

void timecontrol(const nlohmann::json &j, match::Settings &ms) {
    // Default
    ms.tc.type = SearchSettings::Type::Movetime;
    ms.tc.movetime = 10;

    if (j.find("timecontrol") == j.end()) {
        std::cerr << "No timecontrol setting found, using default" << std::endl;
        return;
    }

    const auto tc = j.at("timecontrol");

    if (tc.find("movetime") != tc.end()) {
        ms.tc.type = SearchSettings::Type::Movetime;
        ms.tc.movetime = tc.at("movetime");
    } else if (tc.find("nodes") != tc.end()) {
        ms.tc.type = SearchSettings::Type::Nodes;
        ms.tc.nodes = tc.at("nodes");
    } else if (tc.find("time") != tc.end()) {
        ms.tc.type = SearchSettings::Type::Time;
        ms.tc.btime = tc.at("time");
        ms.tc.wtime = tc.at("time");
        if (tc.find("increment") != tc.end()) {
            ms.tc.binc = tc.at("increment");
            ms.tc.winc = tc.at("increment");
        } else if (tc.find("inc") != tc.end()) {
            ms.tc.binc = tc.at("inc");
            ms.tc.winc = tc.at("inc");
        } else {
            ms.tc.binc = 0;
            ms.tc.winc = 0;
        }
    } else if (tc.find("depth") != tc.end()) {
        ms.tc.type = SearchSettings::Type::Depth;
        ms.tc.ply = tc.at("depth");
    } else {
        std::cerr << "No timecontrol setting found, using default" << std::endl;
    }
}

}  // namespace parse::json
