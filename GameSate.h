#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <iostream>
#include <stdexcept> // For std::out_of_range
#include "Player.h"
#include "GameBoard.h"

using namespace std;

/**
 * Represents the state of a game.
 *
 * @tparam Width  The width of the game board.
 * @tparam Height The height of the game board.
 * @tparam MaxTokensPerPlayer The maximum number of tokens each player can have.
 */
template <size_t Width, size_t Height, size_t MaxTokensPerPlayer>
class GameState
{
private:
    GameBoard<Width, Height> board;        // The game board
    Player<MaxTokensPerPlayer> players[2]; // Array of players
    int currentPlayer;                     // Index of the current player

public:
    // Constructor
    GameState() : currentPlayer(0)
    {
        // Initialize the players using the existing constructor
        players[0] = Player<MaxTokensPerPlayer>(0); // Player 0
        players[1] = Player<MaxTokensPerPlayer>(1); // Player 1

        // Initialize the game board
        board = GameBoard<Width, Height>();

        // Add tokens to the players and place them on the board
        for (size_t i = 0; i < MaxTokensPerPlayer; ++i)
        {
            // Create tokens for each player
            Token *token1 = new Token(0, i + 1, players[0].getPlayerNumber()); // Left border excluding corners
            Token *token2 = new Token(i + 1, 0, players[1].getPlayerNumber()); // Top border excluding corners

            // Add tokens to the players
            players[0].addToken(token1);
            players[1].addToken(token2);

            // Place tokens on the board
            board.placeToken(token1);
            board.placeToken(token2);
        }
    }

    // Destructor
    ~GameState()
    {
        cout << "Destroying GameState..." << endl;

        cout << "GameState destroyed." << endl;
    }

    // Copy constructor
    GameState(const GameState &other) : currentPlayer(other.currentPlayer)
    {
        // Copy the game board
        board = other.board;

        // Copy the players
        players[0] = other.players[0];
        players[1] = other.players[1];
    }

    // Get the current player
    Player<MaxTokensPerPlayer> &getCurrentPlayer()
    {
        return players[currentPlayer];
    }

    // Get the other player
    Player<MaxTokensPerPlayer> &getOtherPlayer()
    {
        return players[1 - currentPlayer];
    }

    // Switch to the other player
    void switchPlayer()
    {
        currentPlayer = 1 - currentPlayer;
    }

    // Get the game board
    GameBoard<Width, Height> &getBoard()
    {
        return board;
    }

    // Move a token on the board
    void moveToken(int fromX, int fromY, int toX, int toY)
    {
        board.moveToken(fromX, fromY, toX, toY);
    }

    // Print the game board
    void printBoard()
    {
        board.printBoard();
    }
};

#endif // GAMESTATE_H