#ifndef LIBATAXX_PGN_HPP
#define LIBATAXX_PGN_HPP

#include <chrono>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "position.hpp"

namespace libataxx {

namespace pgn {

struct GameNode {
    [[nodiscard]] bool is_root() const {
        return parent == nullptr;
    }

    [[nodiscard]] bool has_children() const {
        return !children_.empty();
    }

    [[nodiscard]] bool has_comment() const {
        return !comment_.empty();
    }

    void add_comment(const std::string &str) {
        comment_ = str;
    }

    GameNode *add_mainline(const Move move) {
        children_.insert(
            children_.begin(),
            GameNode{.move_ = move, .ply = this->ply + 1, .parent = this});
        return &children_.front();
    }

    GameNode *add_variation(const Move move) {
        if (has_children()) {
            children_.push_back(
                GameNode{.move_ = move, .ply = this->ply, .parent = this});
        } else {
            children_.push_back(
                GameNode{.move_ = move, .ply = this->ply + 1, .parent = this});
        }
        return &children_.back();
    }

    // Data
    Move move_{};
    int ply{-1};
    std::string comment_{};
    // Tree
    GameNode *parent{};
    std::vector<GameNode> children_{};
};

struct Game {
    Game() : header{{"Event", "?"}, {"Result", "*"}}, position{} {
    }

    Game(const Position &pos)
        : header{{"Event", "?"}, {"Result", "*"}}, position{pos} {
        GameNode *node = &root;
        for (const auto &move : pos.history()) {
            node = node->add_mainline(move);
            assert(node);
        }
    }

    [[nodiscard]] static Game from_position(const Position &pos) {
        return Game{.position = pos};
    }

    std::unordered_map<std::string, std::string> header;
    Position position;
    GameNode root;
};

std::basic_ostream<char> &operator<<(std::basic_ostream<char> &os,
                                     const GameNode &root) {
    const GameNode *node = &root;

    if (node->is_root() && node->is_root()) {
        node = &node->children_[0];
    }

    while (node) {
        // Move number
        if (node->ply % 2 == 0 || node == &root || node->is_root() ||
            node->parent->children_.size() > 1) {
            if (node->ply % 2 == 0) {
                os << (node->ply / 2 + 1) << ". ";
            } else {
                os << (node->ply / 2 + 1) << "... ";
            };
        }

        // Move
        os << node->move_;

        // Comment
        if (node->has_comment()) {
            os << " {" << node->comment_ << "}";
        }

        // Variations
        for (std::size_t i = 1; i < node->children_.size(); ++i) {
            os << " (" << node->children_[i] << ")";
        }

        // Spacer
        if (node->has_children()) {
            os << " ";
        }

        if (node->has_children()) {
            node = &node->children_[0];
        } else {
            break;
        }
    }

    return os;
}

std::basic_ostream<char> &operator<<(std::basic_ostream<char> &os,
                                     const Game &game) {
    static const std::string header_order[] = {"Event",
                                               "Site",
                                               "Date",
                                               "Round",
                                               "Black",
                                               "White",
                                               "Result",
                                               "FEN",
                                               "TimeControl"};

    // Header
    for (const auto &word : header_order) {
        const auto iter = game.header.find(word);
        if (iter != game.header.end()) {
            os << "[" << iter->first << " \"" << iter->second << "\"]"
               << std::endl;
        }
    }
    os << std::endl;

    // Moves
    os << game.root;

    if (game.header.find("Result") != game.header.end()) {
        os << " " << game.header.at("Result") << std::endl;
    }

    os << std::endl;

    return os;
}

}  // namespace pgn

}  // namespace libataxx

#endif
