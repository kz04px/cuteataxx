#ifndef MATCH_WORKER_HPP
#define MATCH_WORKER_HPP

#include <memory>
#include <queue>
#include <string>
#include <vector>
#include "tournament/generator.hpp"

class Settings;
class Results;
class GameSettings;

void worker(const Settings &settings,
            const std::vector<std::string> &openings,
            std::shared_ptr<TournamentGenerator> game_generator,
            Results &results);

#endif
