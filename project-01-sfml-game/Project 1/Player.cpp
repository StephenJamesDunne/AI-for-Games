#include "Player.h"
#include <iostream>

Player::Player(const std::string& textureFile)
{
    if (!playerTexture.loadFromFile(textureFile))
    {
        std::cout << "Error loading player texture" << std::endl;
    }

	playerSprite = sf::Sprite(playerTexture);
    playerSprite.setPosition(sf::Vector2f(100, 100));
}

void Player::handleInput(sf::Time deltaTime)
{
    sf::Vector2f movement(0.0f, 0.0f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
    {
        movement.y -= 1.0f;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
        movement.y += 1.0f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
		movement.x -= 1.0f;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
	{
        movement.x += 1.0f;
	}

    move(movement, deltaTime);
}

void Player::move(sf::Vector2f direction, sf::Time deltaTime)
{
    sf::Vector2f movement = direction * movementSpeed * deltaTime.asSeconds();
    playerSprite.move(movement);
}

void Player::update(sf::Vector2u windowSize)
{
    wrapAroundScreen(windowSize);
}

void Player::draw(sf::RenderWindow& window)
{
	window.draw(playerSprite);
}

sf::Vector2f Player::getPosition() const
{
    return playerSprite.getPosition();
}

void Player::wrapAroundScreen(sf::Vector2u windowSize)
{
    sf::Vector2f position = playerSprite.getPosition();
    sf::FloatRect bounds = playerSprite.getGlobalBounds();

    if (position.x + bounds.size.x < 0) // right edge of the sprite hits the left of the screen
    {
        playerSprite.setPosition(sf::Vector2f(windowSize.x, position.y));
    }
    else if (position.x > windowSize.x) // left edge of the sprite hits the right of the screen
    {
        playerSprite.setPosition(sf::Vector2f(-bounds.size.x, position.y));
    }

    if (position.y + bounds.size.y < 0) // bottom edge of the sprite hits the top of the screen
    {
        playerSprite.setPosition(sf::Vector2f(position.x, windowSize.y));
    }
    else if (position.y > windowSize.y) // top edge of the sprite hits the bottom of the screen
    {
        playerSprite.setPosition(sf::Vector2f(position.x, -bounds.size.y));
    }
}
