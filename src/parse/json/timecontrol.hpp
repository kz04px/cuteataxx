#ifndef PARSE_JSON_TIMECONTROL_HPP
#define PARSE_JSON_TIMECONTROL_HPP

#include <nlohmann/json.hpp>
#include "../../match/settings.hpp"

namespace parse::json {

void timecontrol(const nlohmann::json &j, match::Settings &ms);

}  // namespace parse::json

#endif
