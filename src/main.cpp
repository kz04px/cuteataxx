#include <iostream>
#include <stdexcept>
#include "match/match.hpp"
#include "match/openings.hpp"
#include "match/settings.hpp"
#include "parse/json/file.hpp"
#include "parse/openings.hpp"
#include "uaiengine.hpp"

using namespace match;

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Must provide path to settings file" << std::endl;
        return 1;
    }

    try {
        Match match;

        // Load settings
        std::cout << "Loading settings from " << argv[1] << std::endl;
        const auto &[settings, engines] = parse::json::file(argv[1]);
        std::cout << "games " << settings.num_games << std::endl;
        std::cout << "engines " << engines.size() << std::endl;
        std::cout << "concurrency " << settings.concurrency << std::endl;
        std::cout << "timeout buffer " << settings.timeout_buffer << "ms" << std::endl;

        switch (settings.tc.type) {
            case SearchSettings::Type::Depth:
                std::cout << "depth " << settings.tc.ply << std::endl;
                break;
            case SearchSettings::Type::Nodes:
                std::cout << "nodes " << settings.tc.nodes << std::endl;
                break;
            case SearchSettings::Type::Movetime:
                std::cout << "movetime " << settings.tc.movetime << "ms" << std::endl;
                break;
            case SearchSettings::Type::Time:
                std::cout << settings.tc.btime << "+" << settings.tc.binc << "ms" << std::endl;
                break;
            default:
                throw std::invalid_argument("Coulldn't find movetime");
        }

        std::cout << std::endl;

        // Load openings
        std::cout << "Loading openings from " << settings.openings_path << std::endl;
        const auto openings = parse::openings(settings.openings_path);
        std::cout << "found " << openings.size() << std::endl;
        std::cout << std::endl;

        // Sanity checks
        if (settings.concurrency > 128) {
            throw "Too many threads fam";
        } else if (engines.size() > 128) {
            throw "Too many engines fam";
        }

        // Clear pgn
        if (settings.pgn_override) {
            std::ofstream file(settings.pgn_path, std::ofstream::trunc);
        }

        // Run match
        std::cout << "Starting games" << std::endl;
        std::cout << std::endl;
        match.run(settings, openings, engines);
    } catch (std::invalid_argument &e) {
        std::cerr << e.what() << std::endl;
    } catch (const char *e) {
        std::cerr << e << std::endl;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Uh oh" << std::endl;
    }

    return 0;
}
