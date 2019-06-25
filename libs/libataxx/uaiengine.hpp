#ifndef LIBATAXX_UAIENGINE_HPP
#define LIBATAXX_UAIENGINE_HPP

#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>
#include <tuple>
#include <vector>
#include "engine.hpp"
#include "move.hpp"
#include "position.hpp"
#include "pv.hpp"
#include "search.hpp"

namespace libataxx {

namespace uai {

struct Score {
    int cs;
    int mate;
};

struct Info {
    int depth;
    int seldepth;
    int time;
    int multipv;
    std::uint64_t nodes;
    std::uint64_t tbhits;
    std::uint64_t nps;
    Score score;
    PV pv;
    Move currmove;
    int currmovenumber;
    float hashfull;
    float cpuload;
    std::string string;
};

class Engine : public EngineBase {
   public:
    Engine(const std::string &path) : EngineBase(path) {
    }

    ~Engine() {
    }

    void uai() {
        if (uaiok) {
            return;
        }
        send("uai\n");
        std::unique_lock<std::mutex> lock(mtx_);
        cv_uaiok_.wait(lock);
        lock.unlock();
        uaiok = true;
    }

    void uainewgame() {
        send("uainewgame\n");
    }

    void setoption(const std::string &name, const std::string &value) {
        send("setoption name " + name + " value " + value);
    }

    void isready() {
        send("isready\n");
        std::unique_lock<std::mutex> lock(mtx_);
        cv_readyok_.wait(lock);
        lock.unlock();
    }

    void position(const libataxx::Position &pos) {
        send("position fen " + pos.fen() + "\n");
    }

    void register_info_handler(
        std::function<void(const std::string &)> info_handler) {
        info_handlers.push_back(info_handler);
    }

    std::tuple<libataxx::Move, libataxx::Move> go(const SearchOptions &opts) {
        switch (opts.type) {
            case SearchType::Depth:
                if (opts.depth < 1) {
                    throw std::invalid_argument("Depth must be >= 1");
                }
                send("go depth " + std::to_string(opts.depth) + "\n");
                break;
            case SearchType::Movetime:
                if (opts.movetime < 1) {
                    throw std::invalid_argument("Movetime must be >= 1");
                }
                send("go movetime " + std::to_string(opts.movetime) + "\n");
                break;
            case SearchType::Nodes:
                if (opts.nodes == 0ULL) {
                    throw std::invalid_argument("Nodes must be >= 1");
                }
                send("go nodes " + std::to_string(opts.nodes) + "\n");
                break;
            case SearchType::Infinite:
                send("go infinite\n");
                break;
            case SearchType::Time:
                send("go btime " + std::to_string(opts.btime) + " wtime " +
                     std::to_string(opts.wtime) + "\n");
                break;
            default:
                throw "asd";
                break;
        }

        start_searching();
        std::unique_lock<std::mutex> lock(mtx_);
        cv_bestmove_.wait(lock);
        lock.unlock();

        stop_searching();
        return {bestmove(), pondermove()};
    }

    int perft(const int depth) {
        if (depth < 1) {
            return 0;
        }
        send("perft " + std::to_string(depth) + "\n");
        std::unique_lock<std::mutex> lock(mtx_);
        cv_nodes_.wait(lock);
        lock.unlock();

        return 1;
    }

    void stop() {
        if (searching()) {
            send("stop\n");
        }
    }

    Protocol protocol() const override {
        return Protocol::UAI;
    }

    void quit() override {
        send("quit\n");
    }

    void recv(const std::string &str) override {
        if (debug()) {
            std::cout << "<" << str << std::endl;
        }
        std::stringstream ss{str};

        std::string word;
        if (ss >> word) {
            if (word == "uaiok") {
                cv_uaiok_.notify_one();
            } else if (word == "readyok") {
                cv_readyok_.notify_one();
            } else if (word == "bestmove") {
                ss >> word;
                bestmove_ = Move::from_san(word);
                // Catch ponder here
                if (ss >> word && word == "ponder") {
                    ss >> word;
                    pondermove_ = Move::from_san(word);
                }
                cv_bestmove_.notify_one();
            } else if (word == "nodes") {
                cv_nodes_.notify_one();
            } else if (word == "id") {
                ss >> word;
                if (word == "name") {
                    std::string name = "";
                    while (ss >> word) {
                        if (name == "") {
                            name = word;
                        } else {
                            name += " " + word;
                        }
                    }
                    set_name(name);
                } else if (word == "author") {
                    std::string author = "";
                    while (ss >> word) {
                        if (author == "") {
                            author = word;
                        } else {
                            author += " " + word;
                        }
                    }
                    set_author(author);
                }
            } else if (word == "info") {
                for (auto &info_handler : info_handlers) {
                    info_handler(str);
                }
            }
        }
    }

   private:
    bool uaiok{false};
    std::mutex mtx_;
    std::condition_variable cv_uaiok_;
    std::condition_variable cv_readyok_;
    std::condition_variable cv_bestmove_;
    std::condition_variable cv_nodes_;
    std::vector<std::function<void(const std::string &)>> info_handlers;
};

}  // namespace uai

}  // namespace libataxx

#endif