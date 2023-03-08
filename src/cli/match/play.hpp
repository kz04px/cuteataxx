#ifndef MATCH_PLAY_HPP
#define MATCH_PLAY_HPP

#include <libataxx/pgn.hpp>
#include <memory>

class Engine;
class Settings;
class GameSettings;

[[nodiscard]] libataxx::pgn::PGN play(const Settings &settings,
                                      const GameSettings &game,
                                      std::shared_ptr<Engine> engine1,
                                      std::shared_ptr<Engine> engine2);

#endif
