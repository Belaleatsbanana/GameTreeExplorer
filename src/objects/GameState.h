#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <iostream>
#include <stdexcept>

#include "Algo.h"
#include "GameBoard.h"
#include "Player.h"

class GameState {
   private:
    size_t MaxTokensPerPlayer;
    GameBoard board;
    Player player1;
    Player player2;
    int currentPlayer;

    void initializeTokens(float cellW, float cellH) {
        for (size_t i = 0; i < MaxTokensPerPlayer; ++i) {
            Token *token1 = new Token(0, i + 1, 0, "rtoken.png", "rtokenAlt.png", cellW, cellH);
            Token *token2 = new Token(i + 1, 0, 1, "gtoken.png", "gtokenAlt.png", cellW, cellH);

            player1.addToken(token1);
            player2.addToken(token2);

            board.placeToken(token1);
            board.placeToken(token2);
        }
    }

   public:
    GameState(float cellW, float cellH, size_t gameSize)
        : MaxTokensPerPlayer(gameSize - 2),
          board(gameSize, gameSize),
          player1(0, MaxTokensPerPlayer),
          player2(1, MaxTokensPerPlayer),
          currentPlayer(0) {
        initializeTokens(cellW, cellH);
    }

    // Delete copy operations
    // GameState(const GameState &) = delete;
    // GameState &operator=(const GameState &) = delete;
    GameState(const GameState &other)
        : MaxTokensPerPlayer(other.MaxTokensPerPlayer),
          board(other.board),
          player1(other.player1.getPlayerNumber(), other.MaxTokensPerPlayer),
          player2(other.player2.getPlayerNumber(), other.MaxTokensPerPlayer),
          currentPlayer(other.currentPlayer) {
        for (size_t i = 0; i < other.player1.getTokens().size(); ++i) {
            Token *original = other.player1.getTokens()[i];
            Token *copy = new Token(*original);
            player1.addToken(copy);
            board.placeToken(copy);
        }

        for (size_t i = 0; i < other.player2.getTokens().size(); ++i) {
            Token *original = other.player2.getTokens()[i];
            Token *copy = new Token(*original);
            player2.addToken(copy);
            board.placeToken(copy);
        }
    }

    Player &getCurrentPlayer() { return currentPlayer == 0 ? player1 : player2; }
    Player &getOtherPlayer() { return currentPlayer == 0 ? player2 : player1; }
    GameBoard &getBoard() { return board; }

    void switchPlayer() { currentPlayer = 1 - currentPlayer; }

    void moveToken(int fromX, int fromY, int toX, int toY) {
        board.moveToken(fromX, fromY, toX, toY);
        player1.updateMovableTokens();
        player2.updateMovableTokens();

        if (auto token = board.getTokenAt(toX, toY)) {
            if (token->hasReachedEnd()) {
                getCurrentPlayer().setScore(getCurrentPlayer().getScore() + 1);
            }
        }
    }
};

#endif  // GAMESTATE_H
