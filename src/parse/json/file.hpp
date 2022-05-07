#ifndef PARSE_JSON_FILE_HPP
#define PARSE_JSON_FILE_HPP

#include <string>
#include "../../match/settings.hpp"

namespace parse::json {

Settings file(const std::string &path);

}  // namespace parse::json

#endif
