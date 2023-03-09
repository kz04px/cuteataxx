#ifndef ATAXX_ADJUDICATE_HPP
#define ATAXX_ADJUDICATE_HPP

namespace libataxx {
class Position;
}  // namespace libataxx

[[nodiscard]] auto can_adjudicate_material(const libataxx::Position &pos, const int threshold) -> bool;
[[nodiscard]] auto can_adjudicate_easyfill(const libataxx::Position &pos) -> bool;
[[nodiscard]] auto can_adjudicate_gamelength(const libataxx::Position &pos, const int limit) -> bool;

#endif
