#ifndef MATCH_HPP
#define MATCH_HPP

#include <libataxx/pgn.hpp>
#include <memory>
#include <mutex>
#include <stack>
#include "../cache.hpp"
#include "../uaiengine.hpp"
#include "engines.hpp"
#include "game.hpp"
#include "openings.hpp"
#include "results.hpp"

namespace match {

class Settings;

class Match {
   public:
    Match(const std::size_t cache_size) : m_engine_cache(cache_size) {
    }

    void run(const Settings &settings, const Openings &openings, const Engines &engines);

   private:
    libataxx::pgn::PGN play(const Settings &settings, const Game &game);

    void worker(const Settings &settings, std::stack<Game> &games, Results &results);

   private:
    std::mutex mtx_output_;
    std::mutex mtx_games_;
    Cache<int, std::shared_ptr<UAIEngine>> m_engine_cache;
};

}  // namespace match

#endif
