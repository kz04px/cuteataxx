#ifndef ENGINE_PROCESS_HPP
#define ENGINE_PROCESS_HPP

#include <boost/process.hpp>
#include <libataxx/move.hpp>
#include <libataxx/position.hpp>
#include <string>
#include <string_view>
#include "match/settings.hpp"

class Engine {
   public:
    virtual auto init() -> void = 0;

    [[nodiscard]] virtual auto go(const SearchSettings &settings) -> libataxx::Move = 0;

    virtual auto position(const libataxx::Position &pos) -> void = 0;

    virtual auto set_option(const std::string &name, const std::string &value) -> void = 0;

    virtual void isready() = 0;

    virtual void newgame() = 0;

   protected:
    [[nodiscard]] Engine(const std::string &path)
        : m_child(path, boost::process::std_out > m_out, boost::process::std_in < m_in) {
    }

    virtual ~Engine() {
        if (is_running()) {
            m_in.close();
            m_out.close();
            m_child.wait();
        }
    }

    [[nodiscard]] auto is_running() -> bool {
        return m_child.running();
    }

    virtual void quit() = 0;

    virtual void stop() = 0;

    auto send(const std::string &msg) -> void {
        m_in << msg << std::endl;
    }

    [[nodiscard]] auto get_output() -> std::string {
        std::string line;
        std::getline(m_out, line);
        return line;
    }

   private:
    boost::process::opstream m_in;
    boost::process::ipstream m_out;
    boost::process::child m_child;
};

#endif
