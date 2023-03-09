#include "adjudicate.hpp"
#include <libataxx/position.hpp>

[[nodiscard]] auto can_adjudicate_material(const libataxx::Position &pos, const int threshold) -> bool {
    const auto material_imbalance = pos.get_us().count() - pos.get_them().count();
    return material_imbalance >= threshold;
}

[[nodiscard]] auto can_adjudicate_easyfill(const libataxx::Position &pos) -> bool {
    if (!pos.must_pass()) {
        return false;
    }

    const auto them_stuck = (pos.get_us().singles() | pos.get_us().doubles()) & pos.get_them();
    const auto them_unstuck = pos.get_them() ^ them_stuck;
    assert(pos.get_them() == (them_stuck | them_unstuck));
    const auto their_reach =
        pos.get_reachable((them_stuck.singles() | them_unstuck.doubles()) & pos.get_empty(), pos.get_empty());
    const auto all_them = pos.get_them() | their_reach;
    const auto is_easyfill = (all_them | pos.get_empty()) == all_them;
    const auto is_winning = all_them.count() > pos.get_us().count();
    return is_winning;
}

[[nodiscard]] auto can_adjudicate_gamelength(const libataxx::Position &pos, const int limit) -> bool {
    return pos.get_fullmoves() >= limit;
}
