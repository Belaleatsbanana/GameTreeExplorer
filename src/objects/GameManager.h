
#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <SFML/Graphics.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <iostream>
#include <memory>
#include <ostream>
#include <queue>
#include <stack>
#include "Algo.h"
#include "GameSate.h"
#include "GameBoard.h"

class GameManager
{
private:
    struct GameSettings
    {
        size_t size;
        size_t maxTokens;
        float cellSize;
        sf::VideoMode videoMode;
    };

    GameSettings settings;
    sf::RenderWindow window;
    GameState state;
    bool tokenSelected;
    sf::Vector2i selectedPosition;
    sf::Vector2i possibleMove;

    bool gameWon = false;
    sf::Text winText;
    sf::RectangleShape winOverlay;
    sf::Font font;

    std::string player1Name;
    std::string player2Name;
	std::stack<algo::MoveStep> history;

    void handleTokenSelection(const sf::Vector2i &gridPos)
    {
        try
        {
            if (auto *token = state.getBoard().getTokenAt(gridPos.x, gridPos.y))
            {
                if (token->getPlayer() == state.getCurrentPlayer().getPlayerNumber())
                {
                    tokenSelected = true;
                    selectedPosition = gridPos;
                    calculatePossibleMove(gridPos);
                    return;
                }
            }
            resetSelection();
        }
        catch (const std::exception &ex)
        {
            std::cerr << "Selection error: " << ex.what() << std::endl;
            resetSelection();
        }
    }

    void calculatePossibleMove(const sf::Vector2i &gridPos)
    {
        const int player = state.getCurrentPlayer().getPlayerNumber();
        const sf::Vector2i direction(player == 0 ? 1 : 0, player == 1 ? 1 : 0);
        auto pairMove = state.getBoard().getTokenMove(
            gridPos.x, gridPos.y,
            gridPos.x + direction.x,
            gridPos.y + direction.y);
        possibleMove = sf::Vector2i(pairMove.first, pairMove.second);
    }

    void handleTokenMove(const sf::Vector2i &gridPos)
    {
        try
        {
            state.moveToken(
                selectedPosition.x, selectedPosition.y,
                gridPos.x, gridPos.y);

            checkWinCondition();
            checkOtherPlayerMoves();

            resetSelection();
        }
        catch (const std::exception &ex)
        {
            std::cerr << "Move error: " << ex.what() << std::endl;
            resetSelection();
        }
    }

    void checkWinCondition()
    {
        if (state.getCurrentPlayer().getScore() >= settings.maxTokens)
        {
            gameWon = true;
            setupWinScreen();
        }
    }

    void setupWinScreen()
    {
        // Load font if not already loaded
        if (!font.openFromFile("arial.ttf"))
        {
            std::cerr << "Error loading font for win screen!\n";
            // Handle error
        }

        // Create dark overlay
        winOverlay.setSize(sf::Vector2f(window.getSize()));
        winOverlay.setFillColor(sf::Color(0, 0, 0, 200));

        // Setup win text
        winText.setFont(font);
        winText.setCharacterSize(60);
        winText.setFillColor(sf::Color::Yellow);
        winText.setStyle(sf::Text::Bold);

        // Use player names instead of numbers
        std::string winnerName = state.getCurrentPlayer().getPlayerNumber() == 0
                                     ? player1Name
                                     : player2Name;
        winText.setString(winnerName + " wins!");

        // Center text
        sf::FloatRect textRect = winText.getLocalBounds();
        winText.setOrigin(sf::Vector2f(textRect.size.x / 2.0f,
                                       textRect.size.y / 2.0f));
        winText.setPosition(sf::Vector2f(
            window.getSize().x / 2.0f, window.getSize().y / 2.0f));
    }

    void checkOtherPlayerMoves()
    {
        if (state.getOtherPlayer().getMovableTokens() == 0)
        {
            std::cout << "Other player has no valid moves!\n";
            return;
        }
        state.switchPlayer();
    }

    void resetSelection()
    {
        tokenSelected = false;
        selectedPosition = {-1, -1};
        possibleMove = {-1, -1};
    }

	void handleBotTurn() {
		std::queue<algo::MoveStep> visualizeMoves;
		algo::playNextMove(state, state.getCurrentPlayer(), history, visualizeMoves, 0);	

		algo::MoveStep nextStep;

		const int base_delay_ms = 500;
		const int base_grid = 3;
		const float delay = (base_delay_ms * base_grid) / static_cast<float>(settings.size);

		GameBoard fakeBoard(state.getBoard());
		while (!visualizeMoves.empty()) {
			auto i = visualizeMoves.front();
			visualizeMoves.pop();
			fakeBoard.moveTokenRaw(i.from.first, i.from.second, i.to.first, i.to.second);


            window.clear(sf::Color::White);
			state.getBoard().draw(window, settings.cellSize, settings.cellSize, false);
			fakeBoard.draw(window, settings.cellSize, settings.cellSize, true);
			window.display();
			sf::sleep(sf::microseconds(static_cast<int>(delay)));
		}

		while (!history.empty()) {
			auto i = history.top();
			history.pop();
			nextStep = i;
		}

		state.getBoard().draw(window, settings.cellSize, settings.cellSize, false);

		state.moveToken(nextStep.from.first, nextStep.from.second, nextStep.to.first, nextStep.to.second);

		checkWinCondition();
		checkOtherPlayerMoves();
		return;
	}

    void handleEvents()
    {
		if (state.getCurrentPlayer().getPlayerNumber() == 1)  {
			handleBotTurn();
			return;
		}
		

        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (auto *mousePress = event->getIf<sf::Event::MouseButtonPressed>())
            {
                const auto mousePos = sf::Mouse::getPosition(window);
                const sf::Vector2i gridPos(
                    static_cast<int>(mousePos.x / settings.cellSize),
                    static_cast<int>(mousePos.y / settings.cellSize));

                if (tokenSelected && gridPos == possibleMove)
                {
                    handleTokenMove(gridPos);
                }
                else
                {
                    handleTokenSelection(gridPos);
                }
            }
        }
    }

    void renderSelection()
    {
        if (!tokenSelected)
            return;

        // Selected token indicator
        sf::RectangleShape selection({settings.cellSize, settings.cellSize});
        selection.setPosition(sf::Vector2f(
            selectedPosition.x * settings.cellSize,
            selectedPosition.y * settings.cellSize));
        selection.setFillColor(sf::Color::Transparent);
        selection.setOutlineColor(sf::Color::Yellow);
        selection.setOutlineThickness(3);
        window.draw(selection);

        // Possible move indicator
        if (possibleMove.x >= 0 && possibleMove.y >= 0)
        {
            sf::CircleShape indicator(settings.cellSize / 4);
            indicator.setPosition(sf::Vector2f(
                possibleMove.x * settings.cellSize + settings.cellSize / 4,
                possibleMove.y * settings.cellSize + settings.cellSize / 4));
            indicator.setFillColor(sf::Color(128, 128, 128, 180));
            window.draw(indicator);
        }
    }

public:
    GameManager(size_t gameSize, const std::string &player1, const std::string &player2)
        : settings{
              gameSize,
              gameSize - 2,
              static_cast<float>(600) / gameSize, // Cell size calculated from known window size
              sf::VideoMode({600, 600})},
          window(settings.videoMode, "Token Game"), state(settings.cellSize, settings.cellSize, gameSize), tokenSelected(false), winText(font, "", 30)
    {
        player1Name = player1;
        player2Name = player2;
        window.setFramerateLimit(60);
    }

    void run()
    {
        while (window.isOpen())
        {
			std::cout << "RUNNING NEW LOOOP" << std::endl;
            handleEvents();
			std::cout << "FINSIHED EVENTS" << std::endl;
			
            window.clear(sf::Color::White);
            state.getBoard().draw(window, settings.cellSize, settings.cellSize, false);
            renderSelection();

			std::cout << "FINSIHED DRAWING" << std::endl;

            if (gameWon)
            {
                window.draw(winOverlay);
                window.draw(winText);
            }

            window.display();
            if (gameWon)
            {
                sf::sleep(sf::seconds(3)); // Pause for 3 seconds before closing
                window.close();
            }
        }
    }
};

#endif // GAMEMANAGER_H
