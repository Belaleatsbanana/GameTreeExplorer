#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "GameSate.h"
#include <iostream>

using namespace std;

/**
 * Manages the game loop, player turns, and game state transitions.
 */
template <size_t Width, size_t Height, size_t MaxTokens>
class GameManager
{
private:
    GameState<Width, Height, MaxTokens> state; // The game state

public:
    // Initialize the game
    GameManager() : state()
    {
        }

    // Run the game loop
    void run()
    {

        cout << "Starting the game..." << endl;

        while (true)
        {
            // Print the board
            state.printBoard();

            // Get the current player
            Player<MaxTokens> &player = state.getCurrentPlayer();

            // Get the player's move
            int fromX, fromY, toX, toY;
            cout << "Player " << player.getPlayerNumber() << ", enter move (fromX fromY toX toY): ";
            cin >> fromX >> fromY >> toX >> toY;

            try
            {
                // Move the token
                state.moveToken(fromX, fromY, toX, toY);

                // Switch to the next player
                state.switchPlayer();
            }
            catch (const exception &ex)
            {
                cerr << "Error: " << ex.what() << endl;
            }
        }
    }
};

#endif // GAMEMANAGER_H