#ifndef MATCH_PLAY_HPP
#define MATCH_PLAY_HPP

#include <libataxx/pgn.hpp>

class Settings;
class GameSettings;

libataxx::pgn::PGN play(const Settings &settings, const GameSettings &game);

#endif
