#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "match/run.hpp"
#include "match/settings.hpp"
#include "parse/openings.hpp"
#include "parse/settings.hpp"
#include "uaiengine.hpp"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Must provide path to settings file\n";
        return 1;
    }

    try {
        const auto settings = parse::settings(argv[1]);

        // Check the paths given
        if (!std::filesystem::exists(settings.openings_path)) {
            std::cerr << "Openings path not found " << settings.openings_path << "\n";
            return 1;
        }
        for (const auto &engine : settings.engines) {
            if (!std::filesystem::exists(engine.path)) {
                std::cerr << "Engine path not found " << engine.path << "\n";
                return 1;
            }

            if (engine.proto == EngineProtocol::Unknown) {
                std::cerr << "Unrecognised engine protocol\n";
                return 1;
            }
        }

        const auto openings = parse::openings(settings.openings_path);

        std::cout << "Settings:\n";
        std::cout << "- games " << settings.num_games << "\n";
        std::cout << "- engines " << settings.engines.size() << "\n";
        std::cout << "- concurrency " << settings.concurrency << "\n";
        std::cout << "- timecontrol " << settings.tc << "\n";
        std::cout << "- openings " << openings.size() << "\n";
        std::cout << "\n";

        // Sanity checks
        if (openings.size() < 1) {
            throw std::invalid_argument("Must be at least 1 opening position");
        } else if (settings.engines.size() < 2) {
            throw std::invalid_argument("Must be at least 2 engines");
        } else if (settings.concurrency < 1) {
            throw std::invalid_argument("Must be at least 1 thread");
        }

        // Clear pgn
        if (settings.pgn_override) {
            std::ofstream file(settings.pgn_path, std::ofstream::trunc);
        }

        run(settings, openings);
    } catch (std::invalid_argument &e) {
        std::cerr << e.what() << "\n";
    } catch (const char *e) {
        std::cerr << e << "\n";
    } catch (std::exception &e) {
        std::cerr << e.what() << "\n";
    } catch (...) {
        std::cerr << "Uh oh\n";
    }

    return 0;
}
