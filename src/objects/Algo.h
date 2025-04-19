#ifndef ALGO_H
#define ALGO_H

#include <queue>
#include <stack>
#include <utility>
class GameState;
class Player;
class GameBoard;

namespace algo {
	bool isWinningState(Player &player);

	Player &getOpponent(GameState &state, Player &player);

	std::pair<int, int> calculatePossibleMove(int x, int y, Player &player,
											  GameState &state);
	struct MoveStep {
		std::pair<int, int> from;
		std::pair<int, int> to;
		int playerNumber;
	};

	bool playNextMove(GameState &gameState, Player &player, std::stack<MoveStep> &history, std::queue<MoveStep> &visual, int maxDepth, int moveNum = 0);

	std::pair<int, int> getNextBestMove(GameState &gameState, Player &player);

} // namespace algo

#endif // ALGO_H
