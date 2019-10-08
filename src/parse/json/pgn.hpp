#ifndef PARSE_JSON_PGN_HPP
#define PARSE_JSON_PGN_HPP

#include <nlohmann/json.hpp>
#include "../../match/settings.hpp"

namespace parse {

namespace json {

void pgn(const nlohmann::json &j, match::Settings &ms);

}  // namespace json

}  // namespace parse

#endif
