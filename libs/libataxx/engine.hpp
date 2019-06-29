#ifndef LIBATAXX_ENGINE_HPP
#define LIBATAXX_ENGINE_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/process.hpp>
#include <chrono>
#include <functional>
#include <string>
#include <thread>
#include "move.hpp"

namespace libataxx {

namespace ba = boost::asio;
namespace bp = boost::process;
namespace chr = std::chrono;

enum Protocol : int
{
    UAI = 0
};

class EngineBase {
   public:
    ~EngineBase() {
        auto asd = chr::duration<int, std::milli>(1000);
        const bool r = child_.wait_for(asd);
        if (!r) {
            child_.terminate();
        }
        ios_.stop();
        ios_thread_.join();
    }

    void start() {
        ba::async_read_until(out_,
                             buf_,
                             '\n',
                             boost::bind(&EngineBase::handle_recv,
                                         this,
                                         ba::placeholders::error,
                                         ba::placeholders::bytes_transferred));
    }

    void handle_recv(const boost::system::error_code &ec, std::size_t size) {
        if (!ec) {
            std::istream is(&buf_);
            std::string line;
            std::getline(is, line);
            recv(line);
        }

        start();
    }

    [[nodiscard]] bool running() {
        return child_.running();
    }

    void set_name(const std::string &name) {
        name_ = name;
    }

    void set_author(const std::string &author) {
        author_ = author;
    }

    [[nodiscard]] const std::string &name() const {
        return name_;
    }

    [[nodiscard]] const std::string &author() const {
        return author_;
    }

    [[nodiscard]] bool debug() const {
        return debug_;
    }

    void send(const std::string &line) {
        if (debug()) {
            std::cout << ">" << line << std::endl;
        }
        in_ << line << std::endl;
    }

    virtual void recv(const std::string &str) = 0;

    virtual Protocol protocol() const = 0;

    virtual void quit() = 0;

   protected:
    EngineBase(const std::string &path)
        : out_{ios_}, child_{path, bp::std_out > out_, bp::std_in < in_} {
        start();
        ios_thread_ = std::thread([&]() { ios_.run(); });
    }

   private:
    // Engine
    std::string name_{"Engine"};
    std::string author_{"Author"};
    bool debug_{true};
    // Process
    std::thread ios_thread_;
    ba::io_service ios_;
    ba::streambuf buf_;
    bp::opstream in_;
    bp::async_pipe out_;
    bp::child child_;
};

}  // namespace libataxx

#endif
