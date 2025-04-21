#ifndef ALGO_H
#define ALGO_H

#include <iostream>
#include <ostream>
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

    friend std::ostream &operator <<(std::ostream &out, const MoveStep &nextStep) {
        out << "MoveStep: Player " << nextStep.playerNumber << " (" << nextStep.from.first
                  << "," << nextStep.from.second << ") -> (" << nextStep.to.first << ","
                  << nextStep.to.second << ")"
                  << (nextStep.isForwards ? " (Forward)" : " (Backward)") << std::endl;
		return out;
    }
};

void playNextMove(GameState &state, Player &player, 
                       Stack<algo::MoveStep> &history,
                       std::queue<algo::MoveStep> &visual);
}  // namespace algo

#endif  // ALGO_H
