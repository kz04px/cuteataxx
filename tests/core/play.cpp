#include "core/play.hpp"
#include <doctest/doctest.h>
#include "core/engine/builtin/most_captures.hpp"
#include "core/engine/create.hpp"
#include "core/engine/settings.hpp"

TEST_CASE("Test 1") {
    const auto callbacks = Callbacks{
        .on_engine_start =
            [](const std::string &) {
            },
        .on_game_started =
            [](const std::string &, const std::string &, const std::string &) {
            },
        .on_game_finished =
            [](const libataxx::Result, const std::string &, const std::string &) {
            },
        .on_results_update =
            [](const Results &) {
            },
        .on_info_send =
            [](const std::string &) {
            },
        .on_info_recv =
            [](const std::string &) {
            },
        .on_move =
            [](const libataxx::Move &, const int) {
            },
    };
    const auto settings1 =
        EngineSettings{0, EngineProtocol::Unknown, "Test1", "mostcaptures", "", "", SearchSettings::as_depth(1), {}};
    const auto settings2 =
        EngineSettings{1, EngineProtocol::Unknown, "Test2", "mostcaptures", "", "", SearchSettings::as_depth(1), {}};

    std::shared_ptr<Engine> mostcaptures1;
    mostcaptures1 = make_engine(settings1, {}, {});

    std::shared_ptr<Engine> mostcaptures2;
    mostcaptures2 = make_engine(settings2, {}, {});

    const auto adjudication = AdjudicationSettings{{}, {}, {}, 0};
    const auto game = GameSettings{"startpos", settings1, settings2};

    const auto result1 = play(adjudication, game, mostcaptures1, mostcaptures2, callbacks);
    const auto result2 = play(adjudication, game, mostcaptures2, mostcaptures1, callbacks);

    REQUIRE(result1.endpos.is_gameover());
    REQUIRE(result1.result == result2.result);
    REQUIRE(result1.reason == result2.reason);
    REQUIRE(result1.startpos.get_hash() == result2.startpos.get_hash());
    REQUIRE(result1.endpos.get_hash() == result2.endpos.get_hash());
    REQUIRE(result1.history.size() == result2.history.size());
    REQUIRE(!result1.history.empty());

    // Both games should be identical
    for (std::size_t i = 0; i < result1.history.size(); ++i) {
        REQUIRE(result1.history[i].move == result2.history[i].move);
    }

    // Check endpos is correct according to the history
    auto pos = result1.startpos;
    for (const auto move_info : result1.history) {
        REQUIRE(move_info.movetime >= 0);
        REQUIRE(pos.is_legal_move(move_info.move));
        pos.makemove(move_info.move);
    }
    REQUIRE(pos.get_hash() == result1.endpos.get_hash());
}
