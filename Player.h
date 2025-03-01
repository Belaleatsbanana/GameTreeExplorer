#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include "Token.h"

using namespace std;

/**
 * Represents a player in the game.
 *
 * @tparam MaxTokens The maximum number of tokens a player can have.
 */
template <size_t MaxTokens>
class Player
{
private:
    int playerNumber;         // Unique identifier for the player (e.g., 0 or 1)
    Token *tokens[MaxTokens]; // Fixed-size array of pointers to tokens
    size_t tokenCount;        // Number of tokens currently owned by the player

public:
    // Default constructor
    Player() : playerNumber(0), tokenCount(0)
    {
        // Initialize the tokens array to nullptr
        for (size_t i = 0; i < MaxTokens; ++i)
        {
            tokens[i] = nullptr;
        }
    }

    // Constructor
    Player(int number) : playerNumber(number), tokenCount(0)
    {
        // Initialize the tokens array to nullptr
        for (size_t i = 0; i < MaxTokens; ++i)
        {
            tokens[i] = nullptr;
        }
    }

    // Get the player's number
    int getPlayerNumber() const
    {
        return playerNumber;
    }

    // Add a token to the player's collection
    void addToken(Token *token)
    {
        if (tokenCount >= MaxTokens)
        {
            throw runtime_error("Cannot add more tokens: Maximum token limit reached.");
        }
        tokens[tokenCount++] = token; // Add the token and increment the count
    }

    // Get the player's tokens
    Token **getTokens()
    {
        return tokens;
    }

    // Get the number of tokens owned by the player
    size_t getTokenCount() const
    {
        return tokenCount;
    }

    // Check if the player has any movable tokens
    bool hasMovableTokens() const
    {
        for (size_t i = 0; i < tokenCount; ++i)
        {
            if (tokens[i]->isMovable())
            {
                return true;
            }
        }
        return false;
    }
};

#endif // PLAYER_H