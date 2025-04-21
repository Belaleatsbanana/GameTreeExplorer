#include "Algo.h"

#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>
#include <cstdio>
#include <iostream>
#include <queue>
#include <utility>

#include "GameState.h"
#include "Player.h"
#include "Stack.h"

enum Outcome {
    WON,
    LOSS,
};

bool isWinningState(Player &player) {
    bool reachedEnd = true;
    int n = player.getTokenCount();

    if (player.getPlayerNumber() == 0) {
        for (auto token : player.getTokens()) {
            if (token->getPosition().first != n + 1) {
                reachedEnd = false;
                break;
            }
        }
    } else {
        for (auto token : player.getTokens()) {
            if (token->getPosition().second != n + 1) {
                reachedEnd = false;
                break;
            }
        }
    }

    return reachedEnd;
}

Player &getOpponent(GameState &state, Player &player) {
    if (state.getOtherPlayer().getPlayerNumber() == player.getPlayerNumber())
        return state.getCurrentPlayer();
    else
        return state.getOtherPlayer();
}

std::pair<int, int> calculatePossibleMove(pair<int, int> from, Player &player, GameState &state) {
    int playerNumber = player.getPlayerNumber();
    int x = from.first;
    int y = from.second;

    const sf::Vector2i direction(playerNumber == 0 ? 1 : 0, playerNumber == 1 ? 1 : 0);

    auto pairMove = state.getBoard().getTokenMove(x, y, x + direction.x, y + direction.y);
    return pairMove;
}

Outcome recusionMove(GameState &state, Player &player, Stack<algo::MoveStep> &history,
                     std::queue<algo::MoveStep> &visual, bool &hasWon) {
    if (hasWon) return player.getPlayerNumber() == 1 ? WON : LOSS;

    if (isWinningState(player)) {
        if (player.getPlayerNumber() == 1) hasWon = true;
        return WON;
    }

    if (isWinningState(getOpponent(state, player))) {
        return LOSS;
    }

    for (auto token : player.getTokens()) {
        if (!token->isMovable()) continue;

        std::pair<int, int> oldMove = token->getPosition();

        std::pair<int, int> newMove = calculatePossibleMove(oldMove, player, state);

        if (newMove.first == -1) {
            continue;
        }

        visual.push(algo::MoveStep{oldMove, newMove, player.getPlayerNumber(), true});
        history.push(algo::MoveStep{oldMove, newMove, player.getPlayerNumber(), true});

        state.getBoard().moveTokenRaw(oldMove.first, oldMove.second, newMove.first, newMove.second);
        Outcome result = recusionMove(state, getOpponent(state, player), history, visual, hasWon);
        state.getBoard().moveTokenRaw(newMove.first, newMove.second, oldMove.first, oldMove.second);

        visual.push(algo::MoveStep{newMove, oldMove, player.getPlayerNumber(), false});
        if (result == LOSS || hasWon) {
            if (player.getPlayerNumber() == 1) hasWon = true;
            return WON;
        }

        history.pop();
    }

    return LOSS;
}

void algo::playNextMove(GameState &state, Player &player, Stack<MoveStep> &history,
                        std::queue<MoveStep> &visual) {
    GameState newState = state;
    bool hasFoundWin = false;
    recusionMove(newState, player, history, visual, hasFoundWin);
    if (!hasFoundWin && history.empty()) history.push(visual.front());
}
