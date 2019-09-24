#ifndef WORKER_HPP
#define WORKER_HPP

#include <chrono>
#include <libataxx/uaiengine.hpp>
#include <mutex>
#include <queue>

using clockz = std::chrono::high_resolution_clock;
using ms = std::chrono::milliseconds;

std::mutex work_mutex;
std::mutex file_mutex;

struct Task {
    std::string fen;
    libataxx::engine::SearchSettings opts;
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

        auto engine1 = libataxx::engine::UAIEngine(task.black_player.path);
        engine1.uai();

        auto engine2 = libataxx::engine::UAIEngine(task.white_player.path);
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
        libataxx::engine::SearchSettings tc = task.opts;

        // Create PV
        std::vector<libataxx::Move> pv;

        bool abort = false;

        // Play game
        while (!pos.gameover()) {
            libataxx::engine::UAIEngine *engine =
                pos.turn() == libataxx::Side::Black ? &engine1 : &engine2;

            assert(engine);

            // Set position
            engine->position(pos);

            // Get move
            engine->isready();
            const auto t0 = clockz::now();
            const libataxx::Move bestmove = engine->go(tc);
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
                case libataxx::engine::SearchSettings::Type::Time:
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
                case libataxx::engine::SearchSettings::Type::Movetime:
                    if (diff.count() > task.opts.movetime) {
                        break;
                    }
                    break;
                default:
                    break;
            }

            // Check move
            if (!pos.legal_move(bestmove)) {
                reason = "Illegal move " + std::string(bestmove);
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

            // Add bestmove to PV
            pv.push_back(bestmove);
        }

        if (!abort) {
            result = pos.result();
        }

        // Create pgn
        auto pgn = libataxx::pgn::PGN{};
        pgn.header().add("Black", task.black_player.name);
        pgn.header().add("White", task.white_player.name);
        pgn.header().add("FEN", task.fen);
        switch (task.opts.type) {
            case libataxx::engine::SearchSettings::Type::Time:
                pgn.header().add("Timecontrol",
                                 std::to_string(task.opts.btime) + "+" +
                                     std::to_string(task.opts.binc) + "s");
                break;
            case libataxx::engine::SearchSettings::Type::Movetime:
                pgn.header().add("Movetime",
                                 std::to_string(task.opts.movetime));
                break;
            case libataxx::engine::SearchSettings::Type::Depth:
                pgn.header().add("Depth", std::to_string(task.opts.depth));
                break;
            case libataxx::engine::SearchSettings::Type::Nodes:
                pgn.header().add("Nodes", std::to_string(task.opts.nodes));
                break;
            default:
                break;
        }
        if (result == libataxx::Result::BlackWin) {
            pgn.header().add("Result", "1-0");
        } else if (result == libataxx::Result::WhiteWin) {
            pgn.header().add("Result", "0-1");
        } else if (result == libataxx::Result::Draw) {
            pgn.header().add("Result", "1/2-1/2");
        } else {
            pgn.header().add("Result", "*");
        }
        if (!reason.empty()) {
            pgn.header().add("Reason", reason);
        }

        // Add moves to pgn
        libataxx::pgn::Node *node = pgn.root();
        for (const auto &move : pv) {
            node = node->add_mainline(move);
        }

        // Print to terminal
        std::cout << pgn.header().get("Result").value() << std::endl;
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
