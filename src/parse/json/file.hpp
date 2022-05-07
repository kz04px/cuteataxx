#ifndef PARSE_JSON_FILE_HPP
#define PARSE_JSON_FILE_HPP

#include <string>
#include "../../match/engines.hpp"
#include "../../match/settings.hpp"

namespace parse::json {

std::pair<Settings, Engines> file(const std::string &path);

}  // namespace parse::json

#endif
