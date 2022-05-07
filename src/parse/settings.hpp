#ifndef PARSE_SETTINGS_HPP
#define PARSE_SETTINGS_HPP

#include <string>
#include "../match/settings.hpp"

namespace parse {

[[nodiscard]] Settings settings(const std::string &path);

}  // namespace parse

#endif
