#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <iostream>
#include <stdexcept> // For std::out_of_range
#include "Token.h"
using namespace std;

/**
 * A game board with a fixed size.
 *
 * @tparam Width  The width of the board.
 * @tparam Height The height of the board.
 */
template <size_t Width, size_t Height>
class GameBoard
{
private:
    Token *board[Width][Height]; // 2D array to store pointers to tokens

public:
    // Constructor
    GameBoard()
    {
        // Initialize the board with nullptr (no tokens)
        for (size_t i = 0; i < Width; ++i)
        {
            for (size_t j = 0; j < Height; ++j)
            {
                board[i][j] = nullptr;
            }
        }
    }

    // Destructor
    ~GameBoard()
    {
        // Clean up dynamically allocated tokens
        for (size_t i = 0; i < Width; ++i)
        {
            for (size_t j = 0; j < Height; ++j)
            {
                if (board[i][j] != nullptr)
                {
                    delete board[i][j];
                }
            }
        }
    }

    // Copy constructor
    GameBoard(const GameBoard &other)
    {
        // Copy the contents of the other board
        for (size_t i = 0; i < Width; ++i)
        {
            for (size_t j = 0; j < Height; ++j)
            {
                if (other.board[i][j] != nullptr)
                {
                    board[i][j] = new Token(*other.board[i][j]);
                }
                else
                {
                    board[i][j] = nullptr;
                }
            }
        }
    }

    // Place a token on the board
    void placeToken(int x, int y, int player)
    {
        if (x < 0 || x >= Width || y < 0 || y >= Height)
        {
            throw out_of_range("Position is out of bounds.");
        }
        if (board[x][y] != nullptr)
        {
            throw runtime_error("Position is already occupied.");
        }
        board[x][y] = new Token(x, y, player);
    }

    // Place a token on the board
    void placeToken(Token *token)
    {
        pair<int, int> pos = token->getPosition();
        int x = pos.first;
        int y = pos.second;

        if (x < 0 || x >= Width || y < 0 || y >= Height)
        {
            throw out_of_range("Position is out of bounds.");
        }
        if (board[x][y] != nullptr)
        {
            throw runtime_error("Position is already occupied.");
        }
        board[x][y] = token;
    }

    // Move a token on the board
    void moveToken(int fromX, int fromY, int toX, int toY)
    {
        if (fromX < 0 || fromX >= Width || fromY < 0 || fromY >= Height ||
            toX < 0 || toX >= Width || toY < 0 || toY >= Height)
        {
            throw out_of_range("Position is out of bounds.");
        }
        if (board[fromX][fromY] == nullptr)
        {
            throw runtime_error("No token at the source position.");
        }
        if (board[toX][toY] != nullptr)
        {
            throw runtime_error("Target position is already occupied.");
        }
        if (!board[fromX][fromY]->isMovable())
        {
            throw runtime_error("Token is not movable.");
        }

        // Move the token
        board[toX][toY] = board[fromX][fromY];
        board[fromX][fromY] = nullptr;
        board[toX][toY]->move(toX, toY);
    }

    // Print the board
    void printBoard() const
    {
        for (size_t i = 0; i < Width; ++i)
        {
            for (size_t j = 0; j < Height; ++j)
            {
                if (board[i][j] == nullptr)
                {
                    cout << ". "; // Empty space
                }
                else
                {
                    cout << board[i][j]->getPlayer() << " "; // Display player number
                }
            }
            cout << endl;
        }
    }
};

#endif // GAMEBOARD_H