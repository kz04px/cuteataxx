#include "create.hpp"
#include "builtin/least_captures.hpp"
#include "builtin/most_captures.hpp"
#include "builtin/random.hpp"
#include "engine.hpp"
#include "fairy_stockfish.hpp"
#include "katago.hpp"
#include "settings.hpp"
#include "uaiengine.hpp"

[[nodiscard]] auto make_engine(const EngineSettings &settings,
                               std::function<void(const std::string &msg)> send,
                               std::function<void(const std::string &msg)> recv) -> std::shared_ptr<Engine> {
    std::shared_ptr<Engine> engine;

    if (settings.builtin.empty()) {
        switch (settings.proto) {
            case EngineProtocol::UAI:
                engine = std::make_shared<UAIEngine>(settings.path, settings.arguments, send, recv);
                break;
            case EngineProtocol::FSF:
                engine = std::make_shared<FairyStockfish>(settings.path, settings.arguments, send, recv);
                break;
            case EngineProtocol::KataGo:
                engine = std::make_shared<KataGo>(settings.path, settings.arguments, send, recv);
                break;
            default:
                throw std::invalid_argument("Unknown engine protocol");
        }
    } else {
        if (settings.builtin == "random") {
            engine = std::make_shared<RandomBuiltin>(send, recv);
        } else if (settings.builtin == "mostcaptures" || settings.builtin == "most-captures" ||
                   settings.builtin == "most captures") {
            engine = std::make_shared<MostCapturesBuiltin>(send, recv);
        } else if (settings.builtin == "leastcaptures" || settings.builtin == "least-captures" ||
                   settings.builtin == "least captures") {
            engine = std::make_shared<LeastCapturesBuiltin>(send, recv);
        } else {
            throw std::invalid_argument("Unknown engine builtin");
        }
    }

    engine->init();
    for (const auto &[key, val] : settings.options) {
        engine->set_option(key, val);
    }

    engine->isready();

    return engine;
}
