#include "adjudicate.hpp"
#include <iostream>
#include <libataxx/bitboard.hpp>
#include <libataxx/position.hpp>

[[nodiscard]] auto can_adjudicate_material(const libataxx::Position &pos, const int threshold) -> bool {
    const auto material_imbalance = pos.get_us().count() - pos.get_them().count();
    return material_imbalance >= threshold;
}

[[nodiscard]] auto can_adjudicate_easyfill(const libataxx::Position &pos) -> bool {
    const auto our_reach = (pos.get_us().singles() | pos.get_us().doubles());
    const auto them_stuck = our_reach & pos.get_them();
    const auto them_free = pos.get_them() ^ them_stuck;
    const auto both_reach = pos.get_both().singles() | pos.get_both().doubles();

    // Is the game already over?
    if (pos.is_gameover()) {
        return false;
    }

    // Can we still move?
    if (our_reach & pos.get_empty()) {
        return false;
    }

    // Can they move without releasing us?
    if (!((pos.get_them().singles() | them_free.doubles()) & pos.get_empty())) {
        return false;
    }

    // Pretend they get everything, is it enough?
    if (pos.get_us().count() > pos.get_them().count() + pos.get_empty().count()) {
        return false;
    }

    const auto reachable = [&] {
        auto bb = (pos.get_them().singles() | them_free.doubles()) & pos.get_empty();
        bb |= (bb.singles() | bb.doubles()) & pos.get_empty();
        bb |= (bb.singles() | bb.doubles()) & pos.get_empty();
        bb |= (bb.singles() | bb.doubles()) & pos.get_empty();
        bb |= (bb.singles() | bb.doubles()) & pos.get_empty();
        bb |= (bb.singles() | bb.doubles()) & pos.get_empty();
        bb |= (bb.singles() | bb.doubles()) & pos.get_empty();
        bb |= (bb.singles() | bb.doubles()) & pos.get_empty();
        bb |= (bb.singles() | bb.doubles()) & pos.get_empty();
        bb |= (bb.singles() | bb.doubles()) & pos.get_empty();
        bb |= (bb.singles() | bb.doubles()) & pos.get_empty();
        return bb;
    }();

    const auto reservoirs = (pos.get_empty() | pos.get_them()).singles() & (pos.get_empty() | pos.get_them());
    if (!(reachable & reservoirs)) {
        return false;
    }

    if ((reachable & reservoirs) && pos.get_them().count() + reachable.count() < pos.get_us().count()) {
        return false;
    }

    return true;
}

[[nodiscard]] auto can_adjudicate_gamelength(const libataxx::Position &pos, const int limit) -> bool {
    return pos.get_fullmoves() >= limit;
}
