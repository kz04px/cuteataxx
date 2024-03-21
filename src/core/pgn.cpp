#include "pgn.hpp"
#include <chrono>
#include <format>
#include <fstream>
#include <sstream>

[[nodiscard]] auto result_string(const libataxx::Result result) -> std::string {
    switch (result) {
        case libataxx::Result::BlackWin:
            return "1-0";
        case libataxx::Result::WhiteWin:
            return "0-1";
        case libataxx::Result::Draw:
            return "1/2-1/2";
        default:
            return "*";
    }
}

[[nodiscard]] auto adjudication_string(const ResultReason reason) -> std::string {
    switch (reason) {
        case ResultReason::Normal:
            return "";
        case ResultReason::OutOfTime:
            return "Out of time";
        case ResultReason::MaterialImbalance:
            return "Material imbalance";
        case ResultReason::EasyFill:
            return "Easy fill";
        case ResultReason::Gamelength:
            return "Max game length reached";
        case ResultReason::IllegalMove:
            return "Illegal move";
        default:
            return "*";
    }
}

auto get_pgn(const PGNSettings &settings,
             const std::string &player1,
             const std::string &player2,
             const GameThingy &data) -> std::string {
    std::ostringstream f{};

    const auto material_difference = data.endpos.get_black().count() - data.endpos.get_white().count();

    f << "[Event \"" << settings.event << "\"]\n";
    f << "[Site \"CuteAtaxx\"]\n";
    f << "[Date \"" << std::format("{:%Y-%m-%d %H:%M}", std::chrono::system_clock::now()) << "\"]\n";
    f << "[Round \"1\"]\n";
    f << "[" << settings.colour1 << " \"" << player1 << "\"]\n";
    f << "[" << settings.colour2 << " \"" << player2 << "\"]\n";
    f << "[Result \"" << result_string(data.result) << "\"]\n";
    f << "[FEN \"" << data.startpos.get_fen() << "\"]\n";
    f << "[FinalFEN \"" << data.endpos.get_fen() << "\"]\n";
    if (data.reason != ResultReason::None) {
        f << "[Adjudicated \"" << adjudication_string(data.reason) << "\"]\n";
    }
    if (data.result == libataxx::Result::BlackWin) {
        f << "[Winner \"" << player1 << "\"]\n";
        f << "[Loser \"" << player2 << "\"]\n";
    } else if (data.result == libataxx::Result::WhiteWin) {
        f << "[Winner \"" << player2 << "\"]\n";
        f << "[Loser \"" << player1 << "\"]\n";
    }
    f << "[PlyCount \"" << data.history.size() << "\"]\n";
    f << "[Material \"" << (material_difference >= 0 ? "+" : "") << material_difference << "\"]\n";
    f << "\n";

    const auto white_first = data.startpos.get_turn() == libataxx::Side::White;
    auto ply = 0;

    if (white_first) {
        f << "1... ";
        ply++;
    }

    for (const auto &info : data.history) {
        if (ply % 2 == 0) {
            f << ply / 2 + 1 << ". ";
        }

        f << info.move << " ";

        if (settings.verbose) {
            f << "{";
            f << " movetime " << info.movetime;
            f << " } ";
        }

        ply++;
    }

    f << result_string(data.result) << "\n";

    f << "\n\n";

    return f.str();
}

auto write_as_pgn(const PGNSettings &settings,
                  const std::string &player1,
                  const std::string &player2,
                  const GameThingy &data) -> void {
    std::ofstream f(settings.path, std::fstream::out | std::fstream::app);
    if (!f.is_open()) {
        return;
    }
    std::string pgn = get_pgn(settings, player1, player2, data);
    f << pgn;
    f.close();
}
