#ifndef WORKER_HPP
#define WORKER_HPP

#include <mutex>
#include <queue>

std::mutex work_mutex;
std::mutex file_mutex;

struct Task {
    std::string fen;
    Player black_player;
    Player white_player;
};

void worker(std::queue<Task> &task_queue, std::ofstream &file) {
    auto engine1 = libataxx::uai::Engine("engines/tiktaxx");
    engine1.uai();

    auto engine2 = libataxx::uai::Engine("engines/tiktaxx");
    engine2.uai();

    libataxx::SearchOptions opts{.type = libataxx::SearchType::Movetime,
                                 .movetime = 10};

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

        // Set position
        libataxx::Position pos;
        pos.set_fen(task.fen);

        // Reset engines
        engine1.uainewgame();
        engine2.uainewgame();
        engine1.isready();
        engine2.isready();

        // Play game
        while (!pos.gameover()) {
            libataxx::uai::Engine *engine =
                pos.turn() == libataxx::Side::Black ? &engine1 : &engine2;

            assert(engine);

            // Set position
            engine->position(pos);

            // Get move
            engine->isready();
            const auto [bestmove, pondermove] = engine->go(opts);

            // Check move
            if (!pos.legal_move(bestmove)) {
                break;
            }

            // Make move
            pos.makemove(bestmove);
        }

        // Create pgn
        auto pgn = libataxx::pgn::Game::from_position(pos);
        pgn.header["Black"] = engine1.name();
        pgn.header["White"] = engine2.name();
        pgn.header["FEN"] = task.fen;
        pgn.header["Timecontrol"] = "1+0.01s";
        if (pos.result() == libataxx::Result::BlackWin) {
            pgn.header["Result"] = "1-0";
        } else if (pos.result() == libataxx::Result::WhiteWin) {
            pgn.header["Result"] = "0-1";
        } else if (pos.result() == libataxx::Result::Draw) {
            pgn.header["Result"] = "1/2-1/2";
        } else {
            pgn.header["Result"] = "*";
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
    }

    engine1.quit();
    engine2.quit();
}

#endif
