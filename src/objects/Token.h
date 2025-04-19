#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <SFML/Graphics.hpp>
using namespace std;

class Token
{
private:
    std::pair<int, int> position; // Position on the board
    int player;                   // Player who owns the token
    bool canMove;                 // Whether the token can move
    sf::Texture texture;          // Texture for the token's image
    sf::Sprite sprite;            // Sprite for the token
    float scaleFactor;            // Scale factor for the token
    bool reachedEnd = false;

public:
    // Constructor with coordinates
    Token(int x, int y, int player, const std::string &imagePath, float cellW, float cellH)
        : position(make_pair(x, y)),
          player(player),
          canMove(true),
          texture(),
          sprite(texture) // Initialize sprite with the texture
    {
        if (!texture.loadFromFile(imagePath))
        {
            std::cerr << "Failed to load texture: " << imagePath << std::endl;
        }
        sprite.setTexture(texture, true); // Update sprite with the loaded texture
                                          // Get original texture size
        sf::Vector2u texSize = texture.getSize();

        scaleFactor = std::min(
            (cellW) / static_cast<float>(texSize.x),
            (cellH) / static_cast<float>(texSize.y));

        // Apply scaling
        sprite.setScale(sf::Vector2f(scaleFactor, scaleFactor));
        // Set origin FIRST before scaling
        sprite.setOrigin(sf::Vector2f(texSize.x / 2.0f, texSize.y / 2.0f));
        // Scale the sprite to fit the cell size
        // Calculate scaling to fit 90% of cell
    }

	Token(const Token& other)
		: position(other.position),
		  player(other.player),
		  canMove(other.canMove),
		  texture(),  // Will be copied below
		  sprite(texture),  // Initialize with our texture
		  scaleFactor(other.scaleFactor),
		  reachedEnd(other.reachedEnd)
	{
		// Copy the texture (SFML textures can't be directly copied)
		// if (!texture.loadFromFile(other.texture.copyToImage())) {
		// 	std::cerr << "Failed to copy texture" << std::endl;
		// }
		texture = other.texture;
		
		// Copy sprite properties
		sprite.setTexture(texture, true);
		sprite.setScale(other.sprite.getScale());
		sprite.setOrigin(other.sprite.getOrigin());
		sprite.setPosition(other.sprite.getPosition());
		sprite.setColor(other.sprite.getColor());
	}

    void updatePosition(float cellW, float cellH)
    {
        // Convert grid position to centered pixel coordinates
        float posX = (position.first + 0.5f) * cellW;
        float posY = (position.second + 0.5f) * cellH;
        sprite.setPosition(sf::Vector2f(posX, posY));
    }

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

    // Check if the token has reached the end of the board
    bool hasReachedEnd() const
    {
        return reachedEnd;
    }

    // Set the token as having reached the end of the board
    void tokenReachedEnd()
    {
        reachedEnd = true;

        canMove = false;
    }

	void undoReachedEnd() 
	{
		reachedEnd = false;

		canMove = true;
	}

    // Draw the token on the window
    void draw(sf::RenderWindow &window, float cellWidth, float cellHeight, int opacity)
    {
        updatePosition(cellWidth, cellHeight);
		sprite.setColor(sf::Color(255, 255, 255, opacity));
        window.draw(sprite);
    }
};

#endif // TOKEN_H
