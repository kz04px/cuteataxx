#ifndef PARSE_MOVE_HPP
#define PARSE_MOVE_HPP

#include <libataxx/move.hpp>
#include <string>

[[nodiscard]] auto parse_move(std::string str) -> libataxx::Move;

#endif
