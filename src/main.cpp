#include <iostream>
#include <libataxx/test.hpp>
#include <string>
#include "match.hpp"
#include "settings.hpp"

int main(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-debug") {
            settings::debug = true;
        } else if (std::string(argv[i]) == "-recover") {
            settings::repeat = true;
        } else if (std::string(argv[i]) == "-repeat") {
            settings::repeat = true;
        } else if (std::string(argv[i]) == "-verbose") {
            settings::verbose = true;
        } else {
            std::cerr << "Unknown parameter: " << argv[i] << std::endl;
        }
    }

    libataxx::test();

    match(3, 100);

    return 0;
}
