#ifndef LIBATAXX_SEARCH_HPP
#define LIBATAXX_SEARCH_HPP

#include <cstdint>

namespace libataxx {

enum SearchType : int
{
    Depth = 0,
    Movetime,
    Nodes,
    Time,
    Infinite
};

struct SearchOptions {
    SearchType type;
    // SearchType::Depth
    int depth = -1;
    // SearchType::Movetime
    int movetime = -1;
    // SearchType::Nodes
    std::uint64_t nodes = 0;
    // SearchType::Time
    int btime = -1;
    int wtime = -1;
    int binc = -1;
    int winc = -1;
    int movestogo = -1;
};

}  // namespace libataxx

#endif
