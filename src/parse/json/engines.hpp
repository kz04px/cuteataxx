#ifndef PARSE_JSON_ENGINES_HPP
#define PARSE_JSON_ENGINES_HPP

#include <nlohmann/json.hpp>
#include <vector>
#include "../../match/settings.hpp"

namespace parse::json {

std::vector<EngineSettings> engines(const nlohmann::json &j);

}  // namespace parse::json

#endif
