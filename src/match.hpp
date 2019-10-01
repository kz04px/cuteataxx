#ifndef MATCH_HPP
#define MATCH_HPP

#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include "engine.hpp"

struct MatchSettings {
   public:
    MatchSettings()
        : ratinginterval{10},
          concurrency{1},
          num_games{100},
          debug{false},
          recover{false},
          verbose{false},
          repeat{true},
          engines{},
          openings{} {
    }

    void add_opening(const std::string &fen) {
        openings.push_back(fen);
    }

    void add_engine(const std::string &name, const std::string &path) {
        engines.emplace(name, EngineSettings{path});
    }

    int ratinginterval;
    int concurrency;
    int num_games;
    bool debug;
    bool recover;
    bool verbose;
    bool repeat;
    std::unordered_map<std::string, EngineSettings> engines;
    std::vector<std::string> openings;
};

struct Game {
    Game() {
    }

    Game(const std::string &fen_,
         const std::string &name1_,
         const std::string &name2_)
        : fen{fen_}, name1{name1_}, name2{name2_} {
    }

    std::string fen;
    std::string name1;
    std::string name2;
};

class Match {
   public:
    explicit Match(const MatchSettings &settings)
        : settings_{settings}, games_completed_{0} {
    }

    void add_game(const Game &game) noexcept {
        games_.push(game);
    }

    void stats() const noexcept {
        std::cout << "Openings loaded: " << settings_.openings.size()
                  << std::endl;
        std::cout << "Engines loaded: " << settings_.engines.size()
                  << std::endl;
    }

    void run();

   private:
    void play() noexcept;

   private:
    const MatchSettings settings_;
    std::unordered_map<std::string, EngineScore> scores_;
    std::queue<Game> games_;
    std::mutex mtx_output_;
    std::mutex mtx_games_;
    std::fstream pgn_file_;
    int games_completed_;
};

#endif
