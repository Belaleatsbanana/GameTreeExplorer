#include "Algo.h"
#include "GameSate.h"
#include "Player.h"
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>
#include <cstdio>
#include <iostream>
#include <queue>
#include <stack>
#include <utility>

enum Outcome {
	WON,
	LOSS,
};

bool algo::isWinningState(Player &player) {
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

Player &algo::getOpponent(GameState &state, Player &player) {
	if (state.getOtherPlayer().getPlayerNumber() == player.getPlayerNumber())
		return state.getCurrentPlayer();
	else
		return state.getOtherPlayer();
}

std::pair<int, int> algo::calculatePossibleMove(int x, int y, Player &player, GameState &state) {
	const sf::Vector2i direction(
			player.getPlayerNumber() == 0 ? 1 : 0,
			player.getPlayerNumber() == 1 ? 1 : 0
			);

	auto pairMove = state.getBoard().getTokenMove(
			x, y,
			x + direction.x,
			y + direction.y);
	return pairMove;
}

Outcome recusionMove(GameState &state, Player &player, std::stack<algo::MoveStep> &history, std::queue<algo::MoveStep> &visual, int depth, int moveNum, bool &hasWon) {
	if (hasWon)
		return player.getPlayerNumber() == 1 ? WON : LOSS;


	if (algo::isWinningState(player)) {
		// std::cout << indent << "Player " << player.getPlayerNumber() << ": Has Won in this state!!" << std::endl; 
		if (player.getPlayerNumber() == 1)
			hasWon = true;
		return WON;
	}
	if (algo::isWinningState(algo::getOpponent(state, player))) {
		// std::cout << indent << "Player " << player.getPlayerNumber() << ": Has Lost in this state;-;" << std::endl; 
		return LOSS;
	}


	int childMoveNum = 0;
	for (auto token : player.getTokens()) {
		if (!token->isMovable())
			continue;

		std::pair<int, int> newMove =
			algo::calculatePossibleMove(token->getPosition().first,
					token->getPosition().second, player, state);

		if (newMove.first == -1) {
			// std::cout << "cant move" << std::endl;
			continue;
		}

		std::pair<int, int> oldMove =
			make_pair(token->getPosition().first, token->getPosition().second);

		visual.push(algo::MoveStep {oldMove , newMove, player.getPlayerNumber()});

		state.getBoard().moveTokenRaw(oldMove.first, oldMove.second, newMove.first, newMove.second);
		// std::cout << "---------------------------" << std::endl;
		// std::cout << "\tPlayer " << player.getPlayerNumber() << " - " << maxDepth << std::endl;
		// state.getBoard().printBoard();


		history.push(algo::MoveStep {oldMove, newMove, player.getPlayerNumber()});

		Outcome result = recusionMove(state, algo::getOpponent(state, player), history, visual, depth + 1, childMoveNum++, hasWon);
		state.getBoard().moveTokenRaw(newMove.first, newMove.second, oldMove.first, oldMove.second);

		visual.push(algo::MoveStep {newMove , oldMove, player.getPlayerNumber()});
		//
		// cout << "Revert token of player " << player.getPlayerNumber() << ": (" << oldMove.first << ", " << oldMove.second << ") <- (" << newMove.first << ", " << newMove.second << ")" << endl;
		if (result == LOSS || hasWon) {
			if (player.getPlayerNumber() == 1)
				hasWon = true;
			// std::cout << "Good Branch for player " << player.getPlayerNumber() << std::endl; 
			return WON;
		}

		history.pop();
		//
	}

	// std::cout << indent << "Player " << player.getPlayerNumber() << ": Has no path" << std::endl; 
	return LOSS;
}

bool algo::playNextMove(GameState &state, Player &player, std::stack<MoveStep> &history, std::queue<MoveStep> &visual, int maxDepth = 0, int moveNum) {
	GameState newState = state;
	bool hasFoundWin = false;
	recusionMove(newState, player, history, visual, 0, 1, hasFoundWin);
	return false;
}

