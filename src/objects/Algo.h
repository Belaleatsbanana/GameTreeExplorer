#ifndef ALGO_H
#define ALGO_H

#include <queue>
#include <utility>

#include "Stack.h"

class GameState;
class Player;
class GameBoard;

namespace algo {
struct MoveStep {
    std::pair<int, int> from;
    std::pair<int, int> to;
    int playerNumber;
    bool isForwards;
};

void playNextMove(GameState &gameState, Player &player, Stack<MoveStep> &history,
                  std::queue<MoveStep> &visual);
}  // namespace algo

#endif  // ALGO_H
