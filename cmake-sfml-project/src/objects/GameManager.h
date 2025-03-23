#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <SFML/Graphics.hpp>
#include "GameSate.h"
#include <iostream>

template <size_t Width, size_t Height, size_t MaxTokens>
class GameManager
{
private:
    sf::RenderWindow window;
    GameState<Width, Height, MaxTokens> state;
    float cellSize;
    bool tokenSelected;
    std::pair<int, int> selectedPosition;

    void handleMouseClick(int mouseX, int mouseY)
    {
        const unsigned gridX = mouseX / cellSize;
        const unsigned gridY = mouseY / cellSize;

        try
        {
            if (!tokenSelected)
            {
                // Select token if it belongs to current player
                Token *token = state.getBoard().getTokenAt(gridX, gridY);
                if (token && token->getPlayer() == state.getCurrentPlayer().getPlayerNumber())
                {
                    tokenSelected = true;
                    selectedPosition = {gridX, gridY};
                }
            }
            else
            {
                // Move selected token based on player direction
                int dx = 0, dy = 0;
                if (state.getCurrentPlayer().getPlayerNumber() == 0)
                {
                    dx = 1; // Player 1 (top) moves down
                }
                else
                {
                    dy = 1; // Player 2 (left) moves right
                }

                state.moveToken(
                    selectedPosition.first,
                    selectedPosition.second,
                    selectedPosition.first + dx,
                    selectedPosition.second + dy);

                tokenSelected = false;
                state.switchPlayer();
            }
        }
        catch (const std::exception &ex)
        {
            std::cerr << "Move error: " << ex.what() << std::endl;
            tokenSelected = false;
        }
    }

    void handleEvents()
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (auto *mousePress = event->getIf<sf::Event::MouseButtonPressed>())
            {
                handleMouseClick(mousePress->position.x, mousePress->position.y);
            }
        }
    }

    void render()
    {
        window.clear();
        state.getBoard().draw(window, cellSize, cellSize);

        // Draw selection indicator
        if (tokenSelected)
        {
            sf::RectangleShape selection(sf::Vector2f(cellSize, cellSize));
            selection.setPosition(sf::Vector2f(
                selectedPosition.first * cellSize,
                selectedPosition.second * cellSize));
            selection.setFillColor(sf::Color::Transparent);
            selection.setOutlineColor(sf::Color::Yellow);
            selection.setOutlineThickness(3);
            window.draw(selection);
        }

        window.display();
    }

public:
    GameManager()
        : window(sf::VideoMode({600, 600}), "Game"),
          cellSize(static_cast<float>(window.getSize().x) / Width),
          state(static_cast<float>(window.getSize().x) / Width,
                static_cast<float>(window.getSize().y) / Height),
          tokenSelected(false)
    {
        window.setFramerateLimit(60);
    }

    void run()
    {
        while (window.isOpen())
        {
            handleEvents();
            render();
        }
    }
};

#endif // GAMEMANAGER_H