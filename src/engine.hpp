#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <string>

struct EngineSettings {
   public:
    EngineSettings(const std::string &p) : path{p} {
    }

    const std::string path;
};

struct EngineScore {
   public:
    EngineScore() : wins{0}, draws{0}, losses{0}, crashes{0} {
    }

    int wins;
    int draws;
    int losses;
    int crashes;
};

inline std::ostream &operator<<(std::ostream &os, const EngineScore &score) {
    os << "+" << score.wins;
    os << "-" << score.losses;
    os << "=" << score.draws;
    return os;
}

#endif
