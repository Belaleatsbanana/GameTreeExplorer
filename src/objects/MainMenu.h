#ifndef MAINMENU_H
#define MAINMENU_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "GameManager.h"

class MainMenu {
   private:
    struct InputField {
        sf::RectangleShape rect;
        sf::Text label;
        sf::Text content;
        bool isActive = false;
    };

    sf::RenderWindow window;
    sf::Font font;

    // Text objects initialized in initializer list
    sf::Text title;
    sf::Text playButton;
    sf::Text exitButton;

    // Input fields
    InputField player1Field;
    InputField player2Field;
    InputField boardSizeField;

    // Toggle boxes: None, Token, Tree
    sf::Text visualLabel;
    std::vector<sf::RectangleShape> boxes;
    std::vector<sf::Text> toggleLabelTexts;  // Added labels for toggle buttons
    int selectedToggle = 0;
    std::vector<std::string> toggleLabels = {"None", "Token", "Tree"};

    sf::RectangleShape inputBackground;
    sf::Clock cursorClock;
    bool showCursor = true;

    GameManager::VisualizationMode stringToMode(const std::string &modeStr) {
        if (modeStr == "None") return GameManager::NONE;
        if (modeStr == "Token") return GameManager::TOKEN;
        if (modeStr == "Tree") return GameManager::GRAPH;
        throw std::invalid_argument("Invalid visualization mode: " + modeStr);
    }

    void initializeText(sf::Text &text, const std::string &str, float yPos) {
        text.setString(str);
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(sf::Vector2f(bounds.size.x / 2.f, bounds.size.y / 2.f));
        text.setPosition(sf::Vector2f(static_cast<float>(window.getSize().x) / 2.f, yPos));
        text.setFillColor(sf::Color::White);
    }

    void createInputField(InputField &field, float yPos, const std::string &labelStr) {
        field.label.setString(labelStr);
        field.label.setPosition(sf::Vector2f(50.f, yPos - 35.f));

        field.rect.setSize(sf::Vector2f(500.f, 40.f));
        field.rect.setPosition(sf::Vector2f(50.f, yPos));
        field.rect.setFillColor(sf::Color(70, 70, 70));
        field.rect.setOutlineThickness(2.f);
        field.rect.setOutlineColor(sf::Color::Transparent);

        field.content.setPosition(sf::Vector2f(60.f, yPos + 5.f));
    }

    void createBoxes() {
        visualLabel.setPosition(sf::Vector2f(50.f, 450.f - 45.f));

        float startX = 50.f;
        float yPos = 450.f;
        float spacing = 200.f;
        for (int i = 0; i < 3; ++i) {
            sf::RectangleShape box;
            box.setSize(sf::Vector2f(24.f, 24.f));
            box.setFillColor(i == selectedToggle ? sf::Color::White : sf::Color::Transparent);
            box.setOutlineThickness(2.f);
            box.setOutlineColor(sf::Color::White);
            box.setPosition(sf::Vector2f(startX + i * spacing, yPos));
            boxes.push_back(box);

            // Create label for the toggle box
            sf::Text label(font, "", 24);
            label.setFont(font);
            label.setString(toggleLabels[i]);
            label.setCharacterSize(24);
            label.setFillColor(sf::Color::White);
            // Position label next to the box (right-aligned with vertical centering)
            float labelX = box.getPosition().x + box.getSize().x + 20.f;
            float labelY =
                box.getPosition().y + (box.getSize().y - label.getLocalBounds().size.y) / 2.f - 5.f;
            label.setPosition(sf::Vector2f(labelX, labelY));
            toggleLabelTexts.push_back(label);
        }
    }

    sf::Font loadFont() {
        sf::Font localFont;
        if (!localFont.openFromFile("arial.ttf")) {
            throw std::runtime_error("Failed to load font!");
        }
        return localFont;
    }

    std::string getPlayer1Name() const { return player1Field.content.getString(); }
    std::string getPlayer2Name() const { return player2Field.content.getString(); }

    int getBoardSize() const {
        try {
            return std::stoi(boardSizeField.content.getString().toAnsiString()) + 2;
        } catch (...) {
            return 3;
        }
    }

    bool validateInput() {
        bool valid = true;
        auto mark = [&](InputField &f) {
            bool err = f.content.getString().isEmpty();
            f.rect.setOutlineColor(err ? sf::Color::Red : sf::Color::Transparent);
            valid &= !err;
        };
        mark(player1Field);
        mark(player2Field);
        try {
            int sz = std::stoi(boardSizeField.content.getString().toAnsiString());
            if (sz < 3) throw std::runtime_error("");
        } catch (...) {
            boardSizeField.rect.setOutlineColor(sf::Color::Red);
            valid = false;
        }
        return valid;
    }

    void handleMouseClick(const sf::Vector2f &mousePos) {
        // Activate input fields
        auto checkField = [&](InputField &f) {
            bool wasActive = f.isActive;
            f.isActive = f.rect.getGlobalBounds().contains(mousePos);

            if (f.isActive && !wasActive) f.content.setString("");  // Clear content when activated
        };
        checkField(player1Field);
        checkField(player2Field);
        checkField(boardSizeField);

        // Toggle boxes selection
        for (size_t i = 0; i < boxes.size(); ++i) {
            if (boxes[i].getGlobalBounds().contains(mousePos)) {
                selectedToggle = static_cast<int>(i);
                for (size_t j = 0; j < boxes.size(); ++j) {
                    boxes[j].setFillColor(j == i ? sf::Color::White : sf::Color::Transparent);
                }
            }
        }

        // Play button
        if (playButton.getGlobalBounds().contains(mousePos)) {
            if (validateInput()) {
                std::string choice = toggleLabels[selectedToggle];
                std::cout << choice << std::endl;

                const size_t bsize = getBoardSize();
                const std::string player1 = getPlayer1Name();
                const std::string player2 = getPlayer2Name();

                GameManager::VisualizationMode mode = stringToMode(choice);

                GameManager game(bsize, player1, player2, mode);
                game.run();
            }
        }
        // Exit button
        else if (exitButton.getGlobalBounds().contains(mousePos)) {
            window.close();
        }
    }

    void handleTextInput(const sf::Event::TextEntered &event) {
        auto process = [&](InputField &f, bool numOnly = false) {
            if (!f.isActive) return;
            if (event.unicode == 8) {
                std::string s = f.content.getString();
                if (!s.empty()) s.pop_back();
                f.content.setString(s);
            } else if (event.unicode >= 32 && event.unicode < 128) {
                char c = static_cast<char>(event.unicode);
                if (!numOnly || std::isdigit(c)) {
                    f.content.setString(f.content.getString() + c);
                }
            }
        };
        process(player1Field);
        process(player2Field);
        process(boardSizeField, true);
    }

    void handleEvents() {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (auto *mb = event->getIf<sf::Event::MouseButtonPressed>()) {
                handleMouseClick(sf::Vector2f(mb->position.x, mb->position.y));
            }
            if (auto *te = event->getIf<sf::Event::TextEntered>()) {
                handleTextInput(*te);
            }
        }
    }

    void update() {
        if (cursorClock.getElapsedTime().asSeconds() > 0.5f) {
            showCursor = !showCursor;
            cursorClock.restart();
        }
    }

    void render() {
        window.clear(sf::Color(30, 30, 30));
        window.draw(inputBackground);

        // Draw input fields
        auto drawField = [&](InputField &f) {
            window.draw(f.rect);
            window.draw(f.label);
            sf::Text cnt = f.content;
            if (f.isActive && showCursor) cnt.setString(cnt.getString() + "_");
            window.draw(cnt);
        };
        drawField(player1Field);
        drawField(player2Field);
        drawField(boardSizeField);

        // Draw boxes and their labels
        for (auto &b : boxes) window.draw(b);
        for (auto &l : toggleLabelTexts) window.draw(l);

        // Draw buttons and title
        window.draw(title);
        window.draw(visualLabel);
        window.draw(playButton);
        window.draw(exitButton);

        window.display();
    }

   public:
    void run() {
        while (window.isOpen()) {
            handleEvents();
            update();
            render();
        }
    }
    MainMenu()
        : window(sf::VideoMode({600, 625}), "Main Menu"),  // Increased window height
          font(loadFont()),
          title(font, "", 30),
          playButton(font, "", 30),
          exitButton(font, "", 30),
          visualLabel(font, "Visualization Mode:", 24),
          player1Field{sf::RectangleShape(), sf::Text(font, "", 24), sf::Text(font, "Player1", 24)},
          player2Field{sf::RectangleShape(), sf::Text(font, "", 24), sf::Text(font, "Player2", 24)},
          boardSizeField{sf::RectangleShape(), sf::Text(font, "", 24), sf::Text(font, "5", 24)} {
        initializeText(title, "Game Setup", 50.f);
        initializeText(playButton, "Start Game", 520.f);  // Moved down
        initializeText(exitButton, "Exit", 575.f);        // Moved down
        createInputField(player1Field, 150.f, "Player 1 Name:");
        createInputField(player2Field, 250.f, "Player 2 Name:");
        createInputField(boardSizeField, 350.f, "Board Size:");
        createBoxes();
        inputBackground.setSize(sf::Vector2f(580.f, 400.f));
        inputBackground.setPosition(sf::Vector2f(10.f, 100.f));
        inputBackground.setFillColor(sf::Color(50, 50, 50, 200));
    }
};

