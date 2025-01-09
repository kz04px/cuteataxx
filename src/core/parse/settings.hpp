#ifndef PARSE_SETTINGS_HPP
#define PARSE_SETTINGS_HPP

#include <string>
#include "../match/settings.hpp"

namespace parse {

enum ParseType
{
    CLI = 0,
    TUI,
};

[[nodiscard]] Settings settings(const std::string &path, const ParseType type);

}  // namespace parse

#endif
