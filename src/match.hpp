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
#include "worker.hpp"

using namespace libataxx;

void match(int concurrency,
           int games,
           std::vector<Player> &players,
           std::vector<std::string> &openings) {
    std::queue<Task> task_queue;

    // Fill task queue
    for (int i = 0; i < games; ++i) {
        const auto task = Task{
            .fen = "x5o/7/7/7/7/7/o5x x",
            .black_player = players[0],
            .white_player = players[1],
        };
        task_queue.push(task);
    }

    std::ofstream file("out.pgn");

    if (!file.is_open()) {
        return;
    }

    const auto t0 = std::chrono::high_resolution_clock::now();

    // Create threads
    std::vector<std::future<void>> futures;
    for (int i = 0; i < concurrency; ++i) {
        futures.push_back(std::async(
            std::launch::async, worker, std::ref(task_queue), std::ref(file)));
    }

    // Wait
    for (int i = 0; i < concurrency; ++i) {
        futures[i].wait();
    }

    const auto t1 = std::chrono::high_resolution_clock::now();
    const auto diff = std::chrono::duration_cast<std::chrono::seconds>(t1 - t0);

    std::cout << "Runtime: " << diff.count() << "ms" << std::endl;
    std::cout << "Games: " << games << std::endl;
}

#endif
