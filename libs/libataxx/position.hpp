#ifndef LIBATAXX_POSITION_HPP
#define LIBATAXX_POSITION_HPP

#include <cstdint>
#include <sstream>
#include <string>
#include "constants.hpp"
#include "move.hpp"
#include "other.hpp"
#include "pv.hpp"

namespace libataxx {

enum Side : bool
{
    Black = 0,
    White
};

enum class Result : int
{
    BlackWin = 0,
    WhiteWin,
    Draw,
    None
};

class Position {
   public:
    Position() {
        pieces[Side::Black] = sq_to_bb(Square::a7) | sq_to_bb(Square::g1);
        pieces[Side::White] = sq_to_bb(Square::a1) | sq_to_bb(Square::g7);
        gaps = sq_to_bb(Square::c3) | sq_to_bb(Square::c5) |
               sq_to_bb(Square::e3) | sq_to_bb(Square::e5);
        turn = Side::Black;
    }

    Position(const std::string &fen) {
        set_fen(fen);
    }

    void set_fen(const std::string &fen) {
        if (fen == "startpos") {
            return set_fen("x5o/7/2-1-2/7/2-1-2/7/o5x x 0 1");
        }

        // Clear board
        pieces[Side::Black] = 0ULL;
        pieces[Side::White] = 0ULL;
        gaps = 0ULL;
        turn = Side::Black;

        std::stringstream ss{fen};
        std::string word;
        int sq = static_cast<int>(Square::a7);

        // Position
        if (ss >> word) {
            for (const auto &c : word) {
                switch (c) {
                    case 'x':
                    case 'X':
                    case 'b':
                    case 'B':
                        pieces[Side::Black] ^= sq_to_bb(sq);
                        sq++;
                        break;
                    case 'o':
                    case 'O':
                    case 'w':
                    case 'W':
                        pieces[Side::White] ^= sq_to_bb(sq);
                        sq++;
                        break;
                    case '-':
                        gaps ^= sq_to_bb(sq);
                        sq++;
                        break;
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                        sq += c - '0';
                        break;
                    case '/':
                        sq -= 14;
                        break;
                    default:
                        return;
                }
            }
        }

        // Turn
        if (ss >> word) {
            if (word == "b" || word == "B" || word == "x" || word == "X") {
                turn = Side::Black;
            } else {
                turn = Side::White;
            }
        }
    }

    int legal_moves(Move *moves) const {
        const Side us = turn;
        const Side them = static_cast<Side>(!us);
        const std::uint64_t filled =
            pieces[Side::Black] | pieces[Side::White] | gaps;
        const std::uint64_t empty = Bitboards::All ^ filled;
        int num_moves = 0;

        // Single moves
        std::uint64_t singles = single_moves(pieces[turn]) & empty;
        while (singles) {
            assert(num_moves < MAX_MOVES);
            const Square to = static_cast<Square>(lsbll(singles));
            moves[num_moves] = Move(to);
            assert(moves[num_moves].type() == MoveType::Single);
            num_moves++;
            singles &= singles - 1;
        }

        // Double moves
        std::uint64_t copy = pieces[turn];
        while (copy) {
            const Square from = static_cast<Square>(lsbll(copy));
            std::uint64_t destinations = double_moves(from) & empty;
            while (destinations) {
                assert(num_moves < MAX_MOVES);
                const Square to = static_cast<Square>(lsbll(destinations));
                moves[num_moves] = Move(from, to);
                assert(moves[num_moves].type() == MoveType::Double);
                num_moves++;
                destinations &= destinations - 1;
            }

            copy &= copy - 1;
        }

        return num_moves;
    }

    [[nodiscard]] int score() const {
        return popcountll(pieces[Side::Black]) -
               popcountll(pieces[Side::White]);
    }

    [[nodiscard]] const std::string fen() const {
        std::string fen = "";
        int empty = 0;

        // Board
        int sq = 42;
        while (sq >= 0) {
            const std::uint64_t bb = sq_to_bb(sq);

            if (bb & pieces[Side::Black]) {
                if (empty) {
                    fen += std::to_string(empty);
                    empty = 0;
                }
                fen += "x";
            } else if (bb & pieces[Side::White]) {
                if (empty) {
                    fen += std::to_string(empty);
                    empty = 0;
                }
                fen += "o";
            } else if (bb & gaps) {
                if (empty) {
                    fen += std::to_string(empty);
                    empty = 0;
                }
                fen += "-";
            } else {
                empty += 1;
            }

            if (sq_to_file(sq) == 6) {
                sq -= 14;
                if (empty) {
                    fen += std::to_string(empty);
                    empty = 0;
                }
                if (sq >= -1) {
                    fen += "/";
                }
            }
            sq++;
        }

        // Turn
        if (turn == Side::Black) {
            fen += " b";
        } else {
            fen += " w";
        }

        return fen;
    }

    void print() const {
        int sq = 42;
        while (sq >= 0) {
            const std::uint64_t bb = sq_to_bb(sq);

            if (bb & pieces[Side::Black]) {
                std::cout << "x";
            } else if (bb & pieces[Side::White]) {
                std::cout << "o";
            } else if (bb & gaps) {
                std::cout << " ";
            } else {
                std::cout << "-";
            }

            if (sq_to_file(sq) == 6) {
                sq -= 14;
                std::cout << std::endl;
            }
            sq++;
        }
        std::cout << "Turn: " << (turn == Side::Black ? "b" : "w") << std::endl;
        std::cout << "FEN: " << fen() << std::endl;
    }

    [[nodiscard]] bool legal_move(const Move &move) const {
        return true;
    }

    [[nodiscard]] bool legal_pv(const PV &pv) const {
        Position npos = *this;
        for (const auto &move : pv) {
            if (!npos.legal_move(move)) {
                return false;
            }
            npos.makemove(move);
        }
        return true;
    }

    void makemove(const Move &move) {
        if (!legal_move(move)) {
            return;
        }
        history_.push_back(move);
    }

    void undomove() {
        const Move move = history_.back();
        history_.pop_back();
    }

    [[nodiscard]] Result result() const {
        if (!gameover()) {
            return Result::None;
        }
        return Result::Draw;
    }

    [[nodiscard]] bool gameover() const {
        return pieces[0] == 0ULL || pieces[1] == 0ULL ||
               (gaps | pieces[0] | pieces[1]) == Bitboards::All;
    }

    [[nodiscard]] std::uint64_t perft(const int depth) {
        if (depth < 1) {
            return 1ULL;
        }

        std::uint64_t nodes = 0ULL;

        libataxx::Move moves[256];
        const int num_moves = legal_moves(moves);
        for (int i = 0; i < num_moves; ++i) {
            makemove(moves[i]);
            nodes += perft(depth - 1);
            undomove();
        }

        return nodes;
    }

    [[nodiscard]] const PV &history() const {
        return history_;
    }

   private:
    std::uint64_t pieces[2];
    std::uint64_t gaps : 49;
    int halfmoves_;
    Side turn;
    PV history_;
};

}  // namespace libataxx

#endif
