#ifndef PARSE_JSON_ENGINES_HPP
#define PARSE_JSON_ENGINES_HPP

#include <nlohmann/json.hpp>
#include "../../match/engines.hpp"
#include "../../match/settings.hpp"

namespace parse {

namespace json {

match::Engines engines(const nlohmann::json &j);

}  // namespace json

}  // namespace parse

#endif
