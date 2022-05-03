#ifndef MATCH_RESULTS_HPP
#define MATCH_RESULTS_HPP

#include <iomanip>
#include <map>
#include <string>

namespace match {

struct Score {
    int wins = 0;
    int draws = 0;
    int losses = 0;
    int crashes = 0;
    int played = 0;
};

struct Results {
    int games_played = 0;
    std::map<std::string, Score> scores;
};

inline std::ostream &operator<<(std::ostream &os, const Score &score) {
    const float points = score.wins + static_cast<float>(score.draws) / 2;
    os << score.wins << " - " << score.losses << " - " << score.draws;
    os << std::fixed << std::setprecision(3) << "  [" << points / score.played << "]";
    return os;
}

}  // namespace match

#endif
