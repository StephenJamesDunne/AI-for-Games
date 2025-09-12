#include "Enemy.h"
#include <iostream>

Enemy::Enemy(const std::string& textureFile)
{
    if (!enemyTexture.loadFromFile(textureFile))
    {
        std::cout << "Error loading player texture" << std::endl;
    }

    enemySprite = sf::Sprite(enemyTexture);
    enemySprite.setPosition(sf::Vector2f(700, 300));
}

void Enemy::handleInput(sf::Time deltaTime)
{
    sf::Vector2f movement(0.0f, 0.0f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
    {
        movement.y -= 1.0f;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
    {
        movement.y += 1.0f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        movement.x -= 1.0f;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        movement.x += 1.0f;
    }

    move(movement, deltaTime);
}

void Enemy::move(sf::Vector2f direction, sf::Time deltaTime)
{
    sf::Vector2f movement = direction * movementSpeed * deltaTime.asSeconds();
    enemySprite.move(movement);
}

void Enemy::update(sf::Vector2u windowSize)
{
    wrapAroundScreen(windowSize);
}

void Enemy::draw(sf::RenderWindow& window)
{
    window.draw(enemySprite);
}

sf::Vector2f Enemy::getPosition() const
{
    return enemySprite.getPosition();
}

void Enemy::wrapAroundScreen(sf::Vector2u windowSize)
{
    sf::Vector2f position = enemySprite.getPosition();
    sf::FloatRect bounds = enemySprite.getGlobalBounds();

    if (position.x + bounds.size.x < 0) 
    {
        enemySprite.setPosition(sf::Vector2f(windowSize.x, position.y));
    }
    else if (position.x > windowSize.x) 
    {
        enemySprite.setPosition(sf::Vector2f(-bounds.size.x, position.y));
    }

    if (position.y + bounds.size.y < 0) 
    {
        enemySprite.setPosition(sf::Vector2f(position.x, windowSize.y));
    }
    else if (position.y > windowSize.y) 
    {
        enemySprite.setPosition(sf::Vector2f(position.x, -bounds.size.y));
    }
}
