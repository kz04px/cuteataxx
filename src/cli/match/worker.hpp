#ifndef MATCH_WORKER_HPP
#define MATCH_WORKER_HPP

#include <queue>

class Settings;
class Results;
class GameSettings;

void worker(const Settings &settings, std::queue<GameSettings> &games, Results &results);

#endif
