#include "worker.hpp"
#include <elo.hpp>
#include <iostream>
#include <memory>
#include <mutex>
#include <sprt.hpp>
#include <thread>
#include "../cache.hpp"
#include "../play.hpp"
#include "results.hpp"
#include "settings.hpp"
// Engines
#include "../engine/create.hpp"
#include "../engine/engine.hpp"
// Tournaments
#include "../tournament/generator.hpp"
#include "../tournament/roundrobin.hpp"

std::mutex mtx_output;
std::mutex mtx_games;

void worker(const Settings &settings,
            const std::vector<std::string> &openings,
            std::shared_ptr<TournamentGenerator> game_generator,
            Results &results,
            const Callbacks &callbacks) {
    auto should_stop = false;
    GameInfo game_info;
    Cache<int, std::shared_ptr<Engine>> engine_cache(2);

    while (!should_stop) {
        {
            std::lock_guard<std::mutex> lock(mtx_games);

            // Return if we're out of things to do
            if (game_generator->is_finished()) {
                return;
            }

            // Get the next game to play
            game_info = game_generator->next();
        }

        const auto game = GameSettings(openings[game_info.idx_opening],
                                       settings.engines[game_info.idx_player1],
                                       settings.engines[game_info.idx_player2]);

        callbacks.on_game_started(game.fen, game.engine1.name, game.engine2.name);

        // If the engines we need aren't in the cache, we get nothing
        auto engine1 = engine_cache.get(game.engine1.id);
        auto engine2 = engine_cache.get(game.engine2.id);

        // Free resources by removing any engine processes left in the cache
        engine_cache.clear();

        // Create new engine processes if necessary, knowing we have the resources available
        if (!engine1) {
            callbacks.on_engine_start(game.engine1.name);
            engine1 = make_engine(game.engine1, callbacks.on_info_send, callbacks.on_info_recv);
        }

        if (!engine2) {
            callbacks.on_engine_start(game.engine2.name);
            engine2 = make_engine(game.engine2, callbacks.on_info_send, callbacks.on_info_recv);
        }

        GameThingy game_data;

        // Play the game
        try {
            game_data = play(settings.adjudication, game, *engine1, *engine2, callbacks);
        } catch (std::invalid_argument &e) {
            std::cerr << e.what() << "\n";
        } catch (const char *e) {
            std::cerr << e << "\n";
        } catch (std::exception &e) {
            std::cerr << e.what() << "\n";
        } catch (...) {
            std::cerr << "Error woops\n";
        }

        engine_cache.push(game.engine1.id, *engine1);
        engine_cache.push(game.engine2.id, *engine2);

        (*engine1).reset();
        (*engine2).reset();

        callbacks.on_game_finished(game_data.result, game.engine1.name, game.engine2.name);

        // Results & printing
        {
            std::lock_guard<std::mutex> lock(mtx_output);

            results.games_played++;

            // Update engine results
            results.scores[game.engine1.name].played++;
            results.scores[game.engine2.name].played++;

            switch (game_data.result) {
                case libataxx::Result::BlackWin:
                    results.scores[game.engine1.name].wins++;
                    results.scores[game.engine2.name].losses++;
                    results.black_wins++;
                    break;
                case libataxx::Result::WhiteWin:
                    results.scores[game.engine1.name].losses++;
                    results.scores[game.engine2.name].wins++;
                    results.white_wins++;
                    break;
                case libataxx::Result::Draw:
                    results.scores[game.engine1.name].draws++;
                    results.scores[game.engine2.name].draws++;
                    results.draws++;
                    break;
                default:
                    break;
            }

            // Write to .pgn
            if (settings.pgn.enabled && !settings.pgn.path.empty()) {
                write_as_pgn(settings.pgn, game.engine1.name, game.engine2.name, game_data);
            }

            // Check SPRT stop
            const auto is_sprt_stop = [&settings, &results, &game]() {
                if (!settings.sprt.enabled || !settings.sprt.autostop || settings.engines.size() != 2) {
                    return false;
                }

                const auto w = results.scores.at(settings.engines.at(0).name).wins;
                const auto l = results.scores.at(settings.engines.at(0).name).losses;
                const auto d = results.scores.at(settings.engines.at(0).name).draws;
                const auto llr = sprt::get_llr(w, l, d, settings.sprt.elo0, settings.sprt.elo1);
                const auto lbound = sprt::get_lbound(settings.sprt.alpha, settings.sprt.beta);
                const auto ubound = sprt::get_ubound(settings.sprt.alpha, settings.sprt.beta);

                return llr <= lbound || llr >= ubound;
            }();

            // Stop the match
            should_stop |= is_sprt_stop;

            callbacks.on_results_update(results);
        }
    }
}
