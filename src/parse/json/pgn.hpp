#ifndef PARSE_JSON_PGN_HPP
#define PARSE_JSON_PGN_HPP

#include <nlohmann/json.hpp>
#include "../../match/settings.hpp"

namespace parse::json {

void pgn(const nlohmann::json &j, match::Settings &ms);

}  // namespace parse::json

#endif
