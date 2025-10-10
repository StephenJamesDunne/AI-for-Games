#include "Entity.h"
#include "Math.h"
#include <iostream>

// Visual Studio complains when these values aren't initialised, so I just used some arbitrary values:
Entity::Entity() : sprite(texture), velocity(0.0f, 0.0f), acceleration(2000.0f), deceleration(1000.0f), maxSpeed(600.0f), rotationSpeed(180.0f) {
}

bool Entity::loadTexture(const std::string& textureFile)
{
    if (!texture.loadFromFile(textureFile))
    {
        std::cout << "Error loading texture: " << textureFile << std::endl;
        return false;
    }
    sprite = sf::Sprite(texture);
    return true;
}

void Entity::setPosition(sf::Vector2f position)
{
    sprite.setPosition(position);
}

void Entity::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

sf::Vector2f Entity::getPosition() const
{
    return sprite.getPosition();
}

void Entity::wrapAroundScreen(sf::Vector2u windowSize)
{
    sf::Vector2f position = sprite.getPosition();
    sf::FloatRect bounds = sprite.getGlobalBounds();

    if (bounds.position.x + bounds.size.x < 0)
    {
        sprite.setPosition(sf::Vector2f(windowSize.x + bounds.size.x / 2, position.y));
    }
    else if (bounds.position.x > windowSize.x)
    {
        sprite.setPosition(sf::Vector2f(-bounds.size.x / 2, position.y));
    }

    if (bounds.position.y + bounds.size.y < 0)
    {
        sprite.setPosition(sf::Vector2f(position.x, windowSize.y + bounds.size.y / 2));
    }
    else if (bounds.position.y > windowSize.y)
    {
        sprite.setPosition(sf::Vector2f(position.x, -bounds.size.y / 2));
    }
}

void Entity::centerOrigin()
{
    sf::Vector2u textureSize = texture.getSize();
    sprite.setOrigin(sf::Vector2f(textureSize.x / 2.0f, textureSize.y / 2.0f));
}