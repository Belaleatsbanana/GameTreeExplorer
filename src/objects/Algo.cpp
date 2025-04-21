#include "Algo.h"

#include <queue>

#include "GameState.h"
#include "Player.h"
#include "Stack.h"

enum Outcome { WON, LOSS };

// Helper function to check winning state
bool isWinningState(Player &player) {
    int n = player.getTokenCount();
    if (player.getPlayerNumber() == 0) {
        for (auto token : player.getTokens())
            if (token->getPosition().first != n + 1) return false;
    } else {
        for (auto token : player.getTokens())
            if (token->getPosition().second != n + 1) return false;
    }
    return true;
}

Player &getOpponent(GameState &state, Player &player) {
    return (state.getCurrentPlayer().getPlayerNumber() == player.getPlayerNumber())
               ? state.getOtherPlayer()
               : state.getCurrentPlayer();
}

std::pair<int, int> calculatePossibleMove(std::pair<int, int> from, Player &player,
                                          GameState &state) {
    const int dx = (player.getPlayerNumber() == 0) ? 1 : 0;
    const int dy = (player.getPlayerNumber() == 1) ? 1 : 0;
    return state.getBoard().getTokenMove(from.first, from.second, from.first + dx,
                                         from.second + dy);
}

// Modified to return both outcome and best move
Outcome recursionMove(GameState &state, Player &player, bool &hasWon,
                      std::queue<algo::MoveStep> &moveQueue, Stack<algo::MoveStep> &history) {
    if (hasWon) return WON;

    if (isWinningState(player)) {
        if (player.getPlayerNumber() == 1) hasWon = true;
        return WON;
    }

    if (isWinningState(getOpponent(state, player))) {
        return LOSS;
    }

    for (auto token : player.getTokens()) {
        if (!token->isMovable()) continue;

        auto oldPos = token->getPosition();
        auto newPos = calculatePossibleMove(oldPos, player, state);
        if (newPos.first == -1) continue;

        // Make the move
        state.getBoard().moveTokenRaw(oldPos.first, oldPos.second, newPos.first, newPos.second);
        moveQueue.push(algo::MoveStep{oldPos, newPos, player.getPlayerNumber(), true});
        if (player.getPlayerNumber() == 1)
            history.push(algo::MoveStep{oldPos, newPos, player.getPlayerNumber(), true});

        // Recurse with opponent
        auto opponentOutcome =
            recursionMove(state, getOpponent(state, player), hasWon, moveQueue, history);

        // Undo move
        state.getBoard().moveTokenRaw(newPos.first, newPos.second, oldPos.first, oldPos.second);
        moveQueue.push(algo::MoveStep{newPos, oldPos, player.getPlayerNumber(), false});

        if (opponentOutcome == LOSS) {
            algo::MoveStep bestMove{oldPos, newPos, player.getPlayerNumber(), true};
            if (player.getPlayerNumber() == 1) hasWon = true;  // Stop further exploration
            std::cout << "Good branch for : " << player.getPlayerNumber() << " Move " << bestMove
                      << std::endl;
            return WON;
        } else if (player.getPlayerNumber() == 1) {
            history.pop();
        }
    }
    return LOSS;
}

void algo::playNextMove(GameState &state, Player &player, Stack<algo::MoveStep> &history,
                        std::queue<algo::MoveStep> &visual) {
    GameState stateCopy = state;
    bool hasWon = false;

    auto outcome = recursionMove(stateCopy, player, hasWon, visual, history);

    if (outcome == LOSS) {
        // Fallback logic
        for (auto token : player.getTokens()) {
            if (!token->isMovable()) continue;
            auto oldPos = token->getPosition();
            auto newPos = calculatePossibleMove(oldPos, player, state);
            if (newPos.first != -1) {
                algo::MoveStep fallback{oldPos, newPos, player.getPlayerNumber(), true};
                history.push(fallback);
                break;
            }
        }
    }
}

/**
 * SEQUENCE OF PATTERNS TO GET ALL BRANCHES
 * SEQUENCE 1 (3x3)
 * Fallback: Token 1 -> Token 2 -> Token 3
 * Branching: Token 2
 * Branching: Token 2
 * Branching: Token 3
 * Branching: Token 3
 * Fallback: Token 2
 *
 */
