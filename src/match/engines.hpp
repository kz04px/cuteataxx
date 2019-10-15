#ifndef MATCH_ENGINES_HPP
#define MATCH_ENGINES_HPP

#include <map>
#include <string>

namespace match {

struct Details {
    int id;
    std::string name;
    std::string path;
    std::map<std::string, std::string> options;
};

using Engines = std::map<std::string, Details>;

}  // namespace match

#endif
