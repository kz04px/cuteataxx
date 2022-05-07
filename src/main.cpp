#include <iostream>
#include <stdexcept>
#include "match/run.hpp"
#include "match/settings.hpp"
#include "parse/json/file.hpp"
#include "parse/openings.hpp"
#include "uaiengine.hpp"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Must provide path to settings file\n";
        return 1;
    }

    try {
        // Load settings
        std::cout << "Loading settings from " << argv[1] << "\n";
        const auto settings = parse::json::file(argv[1]);
        std::cout << "games " << settings.num_games << "\n";
        std::cout << "engines " << settings.engines.size() << "\n";
        std::cout << "concurrency " << settings.concurrency << "\n";
        std::cout << "timeout buffer " << settings.timeout_buffer << "ms\n";

        switch (settings.tc.type) {
            case SearchSettings::Type::Depth:
                std::cout << "depth " << settings.tc.ply << "\n";
                break;
            case SearchSettings::Type::Nodes:
                std::cout << "nodes " << settings.tc.nodes << "\n";
                break;
            case SearchSettings::Type::Movetime:
                std::cout << "movetime " << settings.tc.movetime << "ms\n";
                break;
            case SearchSettings::Type::Time:
                std::cout << settings.tc.btime << "+" << settings.tc.binc << "ms\n";
                break;
            default:
                throw std::invalid_argument("Couldn't find movetime");
        }

        std::cout << "\n";

        // Load openings
        std::cout << "Loading openings from " << settings.openings_path << "\n";
        const auto openings = parse::openings(settings.openings_path);
        std::cout << "found " << openings.size() << "\n";
        std::cout << "\n";

        // Sanity checks
        if (settings.concurrency > 128) {
            throw "Too many threads fam";
        } else if (settings.engines.size() > 128) {
            throw "Too many engines fam";
        }

        // Clear pgn
        if (settings.pgn_override) {
            std::ofstream file(settings.pgn_path, std::ofstream::trunc);
        }

        // Run match
        std::cout << "Starting games\n\n";
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
