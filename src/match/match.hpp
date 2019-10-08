#ifndef MATCH_HPP
#define MATCH_HPP

#include <libataxx/pgn.hpp>
#include <mutex>
#include <stack>
#include "engines.hpp"
#include "game.hpp"
#include "openings.hpp"
#include "results.hpp"

namespace match {

class Settings;

class Match {
   public:
    void run(const Settings &settings,
             const Openings &openings,
             const Engines &engines);

   private:
    static libataxx::pgn::PGN play(const Settings &settings, const Game &game);

    void worker(const Settings &settings,
                std::stack<Game> &games,
                Results &results);

   private:
    std::mutex mtx_output_;
    std::mutex mtx_games_;
};

}  // namespace match

#endif
