#ifndef PARSE_JSON_FILE_HPP
#define PARSE_JSON_FILE_HPP

#include <string>
#include "../../match/engines.hpp"
#include "../../match/settings.hpp"

namespace parse {

namespace json {

std::pair<match::Settings, match::Engines> file(const std::string &path);

}  // namespace json

}  // namespace parse

#endif
