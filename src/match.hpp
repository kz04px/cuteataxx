#ifndef MATCH_HPP
#define MATCH_HPP

#include <cassert>
#include <fstream>
#include <future>
#include <iostream>
#include <libataxx/pgn.hpp>
#include <libataxx/uaiengine.hpp>
#include <vector>
#include "player.hpp"
#include "settings.hpp"
#include "worker.hpp"

using namespace libataxx;

void match(const Settings &settings,
           std::vector<Player> &players,
           std::vector<std::string> &openings) {
    std::queue<Task> task_queue;

    // Fill task queue
    for (int i = 0; i < settings.games; ++i) {
        // Get opening fen
        const int idx = i % openings.size();

        // Create task
        const auto task = Task{
            .fen = openings.at(idx),
            .opts = settings.search,
            .black_player = players[0],
            .white_player = players[1],
        };
        task_queue.push(task);
    }

    std::ofstream file(settings.pgn_path);

    if (!file.is_open()) {
        std::cerr << "Could not open pgn file" << std::endl;
        return;
    }

    const auto t0 = std::chrono::high_resolution_clock::now();

    // Create threads
    std::vector<std::future<void>> futures;
    for (int i = 0; i < settings.concurrency; ++i) {
        futures.push_back(std::async(
            std::launch::async, worker, std::ref(task_queue), std::ref(file)));
    }

    // Wait
    for (int i = 0; i < settings.concurrency; ++i) {
        futures[i].wait();
    }

    const auto t1 = std::chrono::high_resolution_clock::now();
    const auto diff = std::chrono::duration_cast<std::chrono::seconds>(t1 - t0);

    std::cout << "Runtime: " << diff.count() << "s" << std::endl;
    std::cout << "Games: " << settings.games << std::endl;
}

#endif
