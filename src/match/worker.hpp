#ifndef MATCH_WORKER_HPP
#define MATCH_WORKER_HPP

#include <stack>

class Settings;
class Results;
class GameSettings;

void worker(const Settings &settings, std::stack<GameSettings> &games, Results &results);

#endif
