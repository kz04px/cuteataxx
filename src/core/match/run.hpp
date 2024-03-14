#ifndef MATCH_RUN_HPP
#define MATCH_RUN_HPP

#include <vector>
#include "results.hpp"
#include "settings.hpp"

class Settings;

Results run(const Settings &settings, const std::vector<std::string> &openings);

#endif
