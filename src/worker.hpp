#ifndef WORKER_HPP
#define WORKER_HPP

#include <chrono>
#include <libataxx/search.hpp>
#include <libataxx/uaiengine.hpp>
#include <mutex>
#include <queue>

using clockz = std::chrono::high_resolution_clock;
using ms = std::chrono::milliseconds;

std::mutex work_mutex;
std::mutex file_mutex;

struct Task {
    std::string fen;
    libataxx::SearchOptions opts;
    Player black_player;
    Player white_player;
};

void worker(std::queue<Task> &task_queue, std::ofstream &file) {
    while (true) {
        Task task;

        // Get task data
        {
            std::lock_guard<std::mutex> lock(work_mutex);
            if (task_queue.empty()) {
                break;
            }
            task = task_queue.front();
            task_queue.pop();
        }

        libataxx::Result result = libataxx::Result::None;
        std::string reason;

        auto engine1 = libataxx::uai::Engine(task.black_player.path);
        engine1.uai();

        auto engine2 = libataxx::uai::Engine(task.white_player.path);
        engine2.uai();

        // Set position
        libataxx::Position pos;
        pos.set_fen(task.fen);

        // Reset engines
        engine1.uainewgame();
        engine2.uainewgame();
        engine1.isready();
        engine2.isready();

        // Set time control
        libataxx::SearchOptions tc = task.opts;

        bool abort = false;

        // Play game
        while (!pos.gameover()) {
            libataxx::uai::Engine *engine =
                pos.turn() == libataxx::Side::Black ? &engine1 : &engine2;

            assert(engine);

            // Set position
            engine->position(pos);

            // Get move
            engine->isready();
            const auto t0 = clockz::now();
            const auto [bestmove, pondermove] = engine->go(tc);
            const auto t1 = clockz::now();
            const auto diff = std::chrono::duration_cast<ms>(t1 - t0);

            // Update time left
            if (pos.turn() == libataxx::Side::Black) {
                tc.btime -= diff.count();
            } else {
                tc.wtime -= diff.count();
            }

            // Check time left
            switch (task.opts.type) {
                case libataxx::SearchType::Time:
                    if (pos.turn() == libataxx::Side::Black) {
                        if (tc.btime < 0) {
                            reason = "Timeout";
                            abort = true;
                            break;
                        }
                    } else {
                        if (tc.wtime < 0) {
                            reason = "Timeout";
                            abort = true;
                            break;
                        }
                    }
                    break;
                case libataxx::SearchType::Movetime:
                    if (diff.count() > task.opts.movetime) {
                        break;
                    }
                    break;
                default:
                    break;
            }

            // Check move
            if (!pos.legal_move(bestmove)) {
                reason = "Illegal move " + bestmove.san();
                abort = true;
                break;
            }

            // End the game early if we ran into an issue
            if (abort) {
                if (pos.turn() == libataxx::Side::Black) {
                    result = libataxx::Result::WhiteWin;
                } else {
                    result = libataxx::Result::BlackWin;
                }
                break;
            }

            // Make move
            pos.makemove(bestmove);
        }

        if (!abort) {
            result = pos.result();
        }

        // Create pgn
        auto pgn = libataxx::pgn::Game::from_position(pos);
        pgn.header["Black"] = task.black_player.name;
        pgn.header["White"] = task.white_player.name;
        pgn.header["FEN"] = task.fen;
        switch (task.opts.type) {
            case libataxx::SearchType::Time:
                pgn.header["Timecontrol"] =
                    std::to_string(task.opts.btime) + "+" +
                    std::to_string(task.opts.binc) + "s";
                break;
            case libataxx::SearchType::Movetime:
                pgn.header["Movetime"] = task.opts.movetime;
                break;
            case libataxx::SearchType::Depth:
                pgn.header["Depth"] = task.opts.depth;
                break;
            case libataxx::SearchType::Nodes:
                pgn.header["Nodes"] = task.opts.nodes;
                break;
            default:
                break;
        }
        if (result == libataxx::Result::BlackWin) {
            pgn.header["Result"] = "1-0";
        } else if (result == libataxx::Result::WhiteWin) {
            pgn.header["Result"] = "0-1";
        } else if (result == libataxx::Result::Draw) {
            pgn.header["Result"] = "1/2-1/2";
        } else {
            pgn.header["Result"] = "*";
        }
        if (!reason.empty()) {
            pgn.header["Reason"] = reason;
        }

        // Print to terminal
        std::cout << pgn.header["Result"] << std::endl;
        std::cout << std::endl;

        // Save pgn
        {
            std::lock_guard<std::mutex> lock(file_mutex);
            if (file.is_open()) {
                file << pgn;
            }
        }

        engine1.quit();
        engine2.quit();
    }
}

#endif
