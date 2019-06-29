#ifndef TEST_HPP
#define TEST_HPP

#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include "position.hpp"

namespace libataxx {

bool test_fen() {
    const std::string tests[] = {"x5o/7/2-1-2/7/2-1-2/7/o5x x",
                                 "x5o/7/2-1-2/7/2-1-2/7/o5x o",
                                 "7/7/7/7/7/7/7 x",
                                 "7/7/7/7/7/7/7 o"};
    for (const auto &fen : tests) {
        Position pos(fen);
        if (pos.fen() != fen) {
            return false;
        }
    }
    return true;
}

bool test_perft() {
    const std::pair<std::string, std::vector<std::uint64_t>> tests[] = {
        {"startpos", {14, 196, 4184, 86528, 2266352}},
        {"x5o/7/7/7/7/7/o5x x", {16, 256, 6460, 155888, 4752668}},
        {"7/7/7/7/7/7/7 x", {0, 0, 0, 0}},
        {"7/7/7/7/ooooooo/ooooooo/xxxxxxx x", {1, 75, 249}},
        {"7/7/7/7/xxxxxxx/xxxxxxx/ooooooo o", {1, 75, 249}},
        {"7/7/7/7/7/7/7 x", {0, 0, 0}},
        {"7/7/7/7/7/7/7 o", {0, 0, 0}},
        {"7/7/7/7/7/7/6x x", {0, 0, 0}},
        {"7/7/7/7/7/7/6x o", {0, 0, 0}},
        {"xxxxxxx/xxxxxxx/xxxxxxx/xxxxxxx/xxxxxxx/xxxxxxx/xxxxxxx", {0, 0, 0}},
        {"ooooooo/ooooooo/ooooooo/ooooooo/ooooooo/ooooooo/ooooooo", {0, 0, 0}},
    };
    for (const auto &[fen, nodes] : tests) {
        Position pos(fen);

        for (unsigned int i = 0; i < nodes.size(); ++i) {
            if (nodes[i] != pos.perft(i + 1)) {
                return false;
            }
        }
    }
    return true;
}

bool test_pv() {
    const std::pair<std::string, PV> legal_tests[] = {
        {"startpos", {Move(Square::g2)}},
        {"startpos", {Move(Square::g2), Move(Square::a1, Square::b3)}}};
    for (const auto &[fen, pv] : legal_tests) {
        Position pos(fen);
        if (!pos.legal_pv(pv)) {
            return false;
        }
    }

    const std::pair<std::string, PV> illegal_tests[] = {
        {"startpos", {Move(Square::g1)}},
        {"startpos", {Move(Square::a2)}},
        {"startpos", {Move(Square::g2), Move(Square::g2)}},
        {"startpos", {Move(Square::g1, Square::e3)}}};
    for (const auto &[fen, pv] : illegal_tests) {
        Position pos(fen);
        if (pos.legal_pv(pv)) {
            return false;
        }
    }

    return true;
}

bool test_gameover() {
    const std::pair<std::string, bool> tests[] = {
        {"startpos", false},
        {"7/7/7/7/7/7/7", true},
        {"7/7/7/7/7/7/x6", true},
        {"7/7/7/7/7/7/o6", true},
        {"7/7/7/7/7/7/ox5", false},
        {"xxxxxxx/xxxxxxx/xxxxxxx/xxxxxxx/xxxxxxx/xxxxxxx/xxxxxxx", true},
        {"ooooooo/ooooooo/ooooooo/ooooooo/ooooooo/ooooooo/ooooooo", true},
        {"xxxxxxx/xxxxxxx/xxxxxxx/xxxxxxx/xxxxxxx/xxxxxxx/ooooooo", true},
        {"ooooooo/ooooooo/ooooooo/ooooooo/ooooooo/ooooooo/xxxxxxx", true},
        {"-------/-------/-------/-------/-------/-------/-------", true},
        {"-------/-------/-------/-------/-------/-------/-----xo", true},
        {"-------/-------/-------/-------/-------/-------/-----xx", true},
        {"-------/-------/-------/-------/-------/-------/-----oo", true},
        {"xxxxxxx/-------/-------/7/-------/-------/ooooooo", true},
        {"7/7/7/7/xxxxxxx/xxxxxxx/ooooooo x", false},
        {"7/7/7/7/xxxxxxx/xxxxxxx/ooooooo o", false},
        {"7/7/7/7/ooooooo/ooooooo/xxxxxxx x", false},
        {"7/7/7/7/ooooooo/ooooooo/xxxxxxx o", false},
    };
    for (const auto &[fen, expected] : tests) {
        Position pos(fen);
        if (pos.gameover() != expected) {
            return false;
        }
    }
    return true;
}

bool test_legal_move() {
    const std::string fens[]{"startpos"};
    for (const auto &fen : fens) {
        Position pos(fen);
        Move moves[max_moves];
        int num_moves = pos.legal_moves(moves);

        for (int to = 0; to < 49; ++to) {
            for (int from = 0; from < 49; ++from) {
                int dx = std::abs(sq_to_file(to) - sq_to_file(from));
                int dy = std::abs(sq_to_rank(to) - sq_to_rank(from));

                Move move;
                if (dx < 2 && dy < 2) {
                    move = Move(static_cast<Square>(to));
                } else {
                    move = Move(static_cast<Square>(from),
                                static_cast<Square>(to));
                }

                bool legal = pos.legal_move(move);
                bool found = false;
                for (int i = 0; i < num_moves; ++i) {
                    if (move == moves[i]) {
                        found = true;
                        break;
                    }
                }

                if (legal != found) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool test_parse_san() {
    const std::array<std::string, 9> moves{
        "b2", "a1b2", "a2b2", "a3b2", "b1b2", "b3b2", "c1b2", "c2b2", "c3b2"};
    for (unsigned int a = 0; a < moves.size() - 1; ++a) {
        for (unsigned int b = a + 1; b < moves.size(); ++b) {
            const Move move1 = Move::from_san(moves[a]);
            const Move move2 = Move::from_san(moves[b]);
            if (move1 != move2) {
                return false;
            }
        }
    }

    const std::string invalid[] = {
        "a8", "a0", "h1", "a2d8", "d0g6", "\n", "cat", "cats", "longlonglong"};
    for (const auto &str : invalid) {
        try {
            auto move = Move::from_san(str);
            return false;
        } catch (...) {
        }
    }

    return true;
}

bool test_result() {
    const std::pair<std::string, Result> tests[] = {
        {"startpos", Result::None},
        {"7/7/7/7/7/7/7", Result::Draw},
        {"x6/7/7/7/7/7/7", Result::BlackWin},
        {"o6/7/7/7/7/7/7", Result::WhiteWin},
        {"xo5/7/7/7/7/7/7", Result::None},
        {"ox5/7/7/7/7/7/7", Result::None},
        {"xxxxxxx/xxxxxxx/xxxxxxx/xxxxxxx/xxxxxxx/xxxxxxx/xxxxxxx",
         Result::BlackWin},
        {"ooooooo/ooooooo/ooooooo/ooooooo/ooooooo/ooooooo/ooooooo",
         Result::WhiteWin},
        {"xxxxxxx/xxxxxxx/xxxxxxx/xxxxooo/ooooooo/ooooooo/ooooooo",
         Result::BlackWin},
        {"xxxxxxx/xxxxxxx/xxxxxxx/xxxoooo/ooooooo/ooooooo/ooooooo",
         Result::WhiteWin},
        {"xxxxxxx/xxxxxxx/xxxxxxx/xxx-ooo/ooooooo/ooooooo/ooooooo",
         Result::Draw},
    };

    for (const auto &[fen, result] : tests) {
        Position pos(fen);
        if (result != pos.result()) {
            return false;
        }
    }

    return true;
}

bool test_nullmove() {
    // Nullmove legality
    const std::pair<std::string, bool> tests[] = {
        {"startpos", false},
        {"7/7/7/7/7/7/7 x", false},
        {"7/7/7/7/7/7/7 o", false},
        {"7/7/7/7/xxxxxxx/xxxxxxx/ooooooo x", false},
        {"7/7/7/7/xxxxxxx/xxxxxxx/ooooooo o", true},
        {"7/7/7/7/ooooooo/ooooooo/xxxxxxx x", true},
        {"7/7/7/7/ooooooo/ooooooo/xxxxxxx o", false},
        {"ooooooo/ooooooo/ooooooo/xxxxxxx/xxxxxxx/xxxxxxx/xxxxxxx x", false},
        {"ooooooo/ooooooo/ooooooo/xxxxxxx/xxxxxxx/xxxxxxx/xxxxxxx o", false},
    };

    for (const auto &[fen, legal] : tests) {
        Position pos;
        pos.set_fen(fen);

        // Does can_move() work?
        if (legal && pos.can_move()) {
            return false;
        }

        // Is nullmove legal?
        if (pos.legal_move(nullmove) != legal) {
            return false;
        }

        // Does movegen return a nullmove?
        if (legal) {
            Move moves[max_moves];
            const int num_moves = pos.legal_moves(moves);
            if (num_moves != 1 || moves[0] != nullmove) {
                return false;
            }
        }
    }

    // Nullmove equality
    if (nullmove != Move::from_san("0000")) {
        return false;
    }

    return true;
}

void test() {
    std::cout << std::boolalpha;
    std::cout << test_fen() << " -- FEN parsing" << std::endl;
    std::cout << test_perft() << " -- Perft" << std::endl;
    std::cout << test_pv() << " -- PV" << std::endl;
    std::cout << test_gameover() << " -- Gameover" << std::endl;
    std::cout << test_nullmove() << " -- Null move" << std::endl;
    std::cout << test_legal_move() << " -- Legal move" << std::endl;
    std::cout << test_parse_san() << " -- Parse san" << std::endl;
    std::cout << test_result() << " -- Result" << std::endl;
}

}  // namespace libataxx

#endif
