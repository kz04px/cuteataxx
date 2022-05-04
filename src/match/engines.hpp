#ifndef MATCH_ENGINES_HPP
#define MATCH_ENGINES_HPP

#include <map>
#include <string>
#include <vector>

namespace match {

struct Details {
    int id;
    std::string name;
    std::string path;
    std::map<std::string, std::string> options;
};

using Engines = std::vector<Details>;

}  // namespace match

#endif
