#ifndef ENGINE_PROCESS_HPP
#define ENGINE_PROCESS_HPP

#include <boost/process.hpp>
#include <functional>
#include <string>
#include <filesystem>
#include "engine.hpp"

class ProcessEngine : public Engine {
   public:
   protected:
    [[nodiscard]] ProcessEngine(const std::string &path,
                                const std::string &arguments,
                                std::function<void(const std::string &msg)> send = {},
                                std::function<void(const std::string &msg)> recv = {})
        : Engine(send, recv),
          m_child(path + (arguments.empty() ? "" : (" " + arguments)),
                  boost::process::start_dir(std::filesystem::path(path).parent_path().string()),
                  boost::process::std_out > m_out,
                  boost::process::std_in < m_in) {
    }

    virtual ~ProcessEngine() {
        if (is_running()) {
            m_in.close();
            m_out.close();
            m_child.wait();
        }
    }

    [[nodiscard]] virtual auto is_running() -> bool override {
        return m_child.running();
    }

    auto send(const std::string &msg) -> void {
        if (m_send) {
            m_send(msg);
        }
        m_in << msg << std::endl;
    }

    [[nodiscard]] auto get_output() -> std::string {
        std::string line;
        std::getline(m_out, line);
        if (m_recv) {
            m_recv(line);
        }
        return line;
    }

   private:
    boost::process::opstream m_in;
    boost::process::ipstream m_out;
    boost::process::child m_child;
};

#endif
