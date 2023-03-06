#ifndef ENGINE_CREATE_HPP
#define ENGINE_CREATE_HPP

#include <functional>
#include <memory>
#include <string>

class Engine;
class EngineSettings;

[[nodiscard]] auto make_engine(const EngineSettings &settings,
                               std::function<void(const std::string &msg)> send = {},
                               std::function<void(const std::string &msg)> recv = {}) -> std::shared_ptr<Engine>;

#endif
