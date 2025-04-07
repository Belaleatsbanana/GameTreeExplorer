#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <iostream>
#include <stdexcept>
#include "Token.h"

class Player
{
private:
    size_t MaxTokens;
    int playerNumber;
    std::vector<Token *> tokens;
    int score;
    int movableTokens;

public:
    // Constructor
    Player(int number, size_t maxTokens)
        : playerNumber(number), MaxTokens(maxTokens), score(0), movableTokens(MaxTokens) {}

    // Destructor to clean up tokens
    ~Player()
    {
        for (auto token : tokens)
        {
            delete token;
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
        if (tokens.size() >= MaxTokens)
        {
            throw std::runtime_error("Cannot add more tokens: Maximum token limit reached.");
        }
        tokens.push_back(token);
    }

    // Get the player's tokens (const version)
    const std::vector<Token *> &getTokens() const
    {
        return tokens;
    }

    // Get the number of tokens owned by the player
    size_t getTokenCount() const
    {
        return tokens.size();
    }

    // Get the player's score
    int getScore() const
    {
        return score;
    }

    // Set the player's score
    void setScore(int newScore)
    {
        score = newScore;
    }

    // Get the number of movable tokens
    int getMovableTokens() const
    {
        return movableTokens;
    }

    // Set the number of movable tokens
    void setMovableTokens(int count)
    {
        movableTokens = count;
    }

    // Check if the player has any movable tokens
    bool hasMovableTokens() const
    {
        for (const auto &token : tokens)
        {
            if (token->isMovable())
            {
                return true;
            }
        }
        return false;
    }

    // Non-const version of getTokens for modification
    std::vector<Token *> &getTokens()
    {
        return tokens;
    }

    // Update the movable tokens based on the game board
    void updateMovableTokens()
    {
        movableTokens = 0;
        for (auto &token : tokens)
        {
            if (token->isMovable())
            {
                ++movableTokens;
            }
        }
    }
};

#endif // PLAYER_H