#ifndef TOKEN_H
#define TOKEN_H

#include <iostream> // For std::pair
using namespace std;

/**
 * A token has a position on the board, a player who owns it, and a flag to indicate whether it can move.
 */

class Token
{
private:
    pair<int, int> position; // Position of the token on the board

    int player;   // Player who owns the token
    bool canMove; // Whether the token can move

public:
    // Constructor
    Token(int x, int y, int player) : position(make_pair(x, y)), player(player), canMove(true) {}

    // Constructor
    Token(pair<int, int> pos, int player) : position(pos), player(player), canMove(true) {}

    // Get the position of the token
    pair<int, int> getPosition() const
    {
        return position;
    }

    // Set the position of the token
    void setPosition(int x, int y)
    {
        position = make_pair(x, y);
    }

    // Get the player who owns the token
    int getPlayer() const
    {
        return player;
    }

    // Check if the token can move
    bool isMovable() const
    {
        return canMove;
    }

    // Set the token to be movable
    void setMovable(bool movable)
    {
        canMove = movable;
    }

    // Move the token to a new position
    void move(int x, int y)
    {
        setPosition(x, y);
    }
};

#endif