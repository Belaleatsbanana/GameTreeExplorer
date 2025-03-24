#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdexcept>
#include <utility>
#include "Token.h"

template <size_t Width, size_t Height>
class GameBoard
{
private:
    Token *board[Width][Height];
    sf::Color borderColor = sf::Color::Black;
    unsigned borderThickness = 2;

    sf::Color getCellColor(size_t row, size_t col)
    {
        // Corner cells
        if ((row == 0 && col == 0) ||
            (row == 0 && col == Width - 1) ||
            (row == Height - 1 && col == 0) ||
            (row == Height - 1 && col == Width - 1))
        {
            return {184, 176, 170}; // Gray
        }
        // Top/bottom borders
        if (row == 0 || row == Height - 1)
        {
            return {210, 241, 210}; // Light green
        }
        // Left/right borders
        if (col == 0 || col == Width - 1)
        {
            return {250, 210, 210}; // Light red
        }
        return sf::Color::White;
    }

    void drawCell(sf::RenderWindow &window, size_t row, size_t col, float cellW, float cellH)
    {
        sf::RectangleShape cell(sf::Vector2f(
            cellW - borderThickness,
            cellH - borderThickness));
        cell.setPosition(sf::Vector2f(
            col * cellW + borderThickness / 2.0f,
            row * cellH + borderThickness / 2.0f));
        cell.setFillColor(getCellColor(row, col));
        window.draw(cell);
    }

    void drawGridLines(sf::RenderWindow &window, float cellW, float cellH)
    {
        // Vertical lines
        for (size_t col = 0; col <= Width; ++col)
        {
            sf::RectangleShape line(sf::Vector2f(
                static_cast<float>(borderThickness),
                Height * cellH));
            line.setPosition(sf::Vector2f(
                col * cellW - borderThickness / 2.0f,
                0.0f));
            line.setFillColor(borderColor);
            window.draw(line);
        }

        // Horizontal lines
        for (size_t row = 0; row <= Height; ++row)
        {
            sf::RectangleShape line(sf::Vector2f(
                Width * cellW,
                static_cast<float>(borderThickness)));
            line.setPosition(sf::Vector2f(
                0.0f,
                row * cellH - borderThickness / 2.0f));
            line.setFillColor(borderColor);
            window.draw(line);
        }
    }

    void drawTokens(sf::RenderWindow &window, float cellW, float cellH)
    {
        for (size_t row = 0; row < Height; ++row)
        {
            for (size_t col = 0; col < Width; ++col)
            {
                if (board[col][row])
                {
                    board[col][row]->draw(window, cellW, cellH);
                }
            }
        }
    }

public:
    GameBoard()
    {
        for (auto &col : board)
        {
            for (auto &cell : col)
            {
                cell = nullptr;
            }
        }
    }

    ~GameBoard()
    {
        for (auto &col : board)
        {
            for (auto &cell : col)
            {
                delete cell;
            }
        }
    }

    GameBoard(const GameBoard &other)
    {
        for (size_t i = 0; i < Width; ++i)
        {
            for (size_t j = 0; j < Height; ++j)
            {
                board[i][j] = other.board[i][j] ? new Token(*other.board[i][j]) : nullptr;
            }
        }
    }

    void placeToken(Token *token)
    {
        const auto [x, y] = token->getPosition();
        board[x][y] = token;
    }

    void moveToken(int fromX, int fromY, int toX, int toY)
    {
        if (fromX < 0 || fromX >= Width || fromY < 0 || fromY >= Height ||
            toX < 0 || toX >= Width || toY < 0 || toY >= Height)
        {
            throw std::out_of_range("Invalid move coordinates");
        }
        if (!board[fromX][fromY])
        {
            throw std::runtime_error("No token at source position");
        }
        if (board[toX][toY])
        {
            if (board[fromX][fromY]->getPlayer() == 0)
            {
                if (board[toX + 1][toY])
                {
                    throw std::runtime_error("Player 1 cannot jump over another token");
                }
                else
                {
                    toX++;
                }
            }
            else if (board[fromX][fromY]->getPlayer() == 1)
            {
                if (board[toX][toY + 1])
                {
                    throw std::runtime_error("Player 2 cannot jump over another token");
                }
                else
                {
                    toY++;
                }
            }
        }
        if (!board[fromX][fromY]->isMovable())
        {
            throw std::runtime_error("Token is immovable");
        }

        board[toX][toY] = board[fromX][fromY];
        board[fromX][fromY] = nullptr;
        board[toX][toY]->move(toX, toY);

        // Check if token has reached the end of the board
        if (toX == 0 || toX == Width - 1 || toY == 0 || toY == Height - 1)
        {
            board[toX][toY]->tokenReachedEnd();
        }
    }

    std::pair<int, int> getTokenMove(int fromX, int fromY, int toX, int toY)
    {
        if (fromX < 0 || fromX >= Width || fromY < 0 || fromY >= Height ||
            toX < 0 || toX >= Width || toY < 0 || toY >= Height)
        {
            return {-1, -1};
        }
        if (!board[fromX][fromY])
        {
            throw std::runtime_error("No token at source position");
        }
        if (board[toX][toY])
        {
            if (board[fromX][fromY]->getPlayer() == 0)
            {
                if (board[toX + 1][toY])
                {
                    return {-1, -1};
                }
                else
                {
                    toX++;
                }
            }
            else if (board[fromX][fromY]->getPlayer() == 1)
            {
                if (board[toX][toY + 1])
                {
                    return {-1, -1};
                }
                else
                {
                    toY++;
                }
            }
        }
        if (!board[fromX][fromY]->isMovable())
        {
            throw std::runtime_error("Token is immovable");
        }
        return {toX, toY};
    }

    bool canTokenMove(Token *token)
    {

        int player = token->getPlayer();

        int toX, toY;
        if (player == 0)
        {
            toX = token->getPosition().first + 1;
            toY = token->getPosition().second;
        }
        else
        {
            toX = token->getPosition().first;
            toY = token->getPosition().second + 1;
        }

        if (toX < 0 || toX >= Width || toY < 0 || toY >= Height)
        {
            return false;
        }
        if (board[toX][toY])
        {
            if (player == 0)
            {
                if (board[toX + 1][toY])
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
            else if (player == 1)
            {
                if (board[toX][toY + 1])
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
        }

        return true;
    }
    void draw(sf::RenderWindow &window, float cellW, float cellH)
    {
        // Draw cells
        for (size_t row = 0; row < Height; ++row)
        {
            for (size_t col = 0; col < Width; ++col)
            {
                drawCell(window, row, col, cellW, cellH);
            }
        }

        // Draw grid lines
        drawGridLines(window, cellW, cellH);

        // Draw tokens
        drawTokens(window, cellW, cellH);
    }

    void printBoard() const
    {
        for (const auto &col : board)
        {
            for (const auto &cell : col)
            {
                std::cout << (cell ? std::to_string(cell->getPlayer()) : ".") << " ";
            }
            std::cout << "\n";
        }
    }

    Token *getTokenAt(int x, int y) const
    {
        if (x < 0 || x >= Width || y < 0 || y >= Height)
        {
            return nullptr;
        }
        return board[x][y];
    }
};

#endif // GAMEBOARD_H