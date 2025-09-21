#include "Enemy.h"
#include <iostream>

Enemy::Enemy(const std::string& textureFile)
{
    if (!enemyTexture.loadFromFile(textureFile))
    {
        std::cout << "Error loading enemy texture" << std::endl;
    }

    enemySprite = sf::Sprite(enemyTexture);
    enemySprite.setPosition(sf::Vector2f(700, 300));

	// center the origin
    sf::Vector2u textureSize = enemyTexture.getSize();
    enemySprite.setOrigin(sf::Vector2f(textureSize.x / 2.0f, textureSize.y / 2.0f));

    velocity = sf::Vector2f(0.0f, 0.0f);
    acceleration = 3500.0f;
    deceleration = 1200.0f;
    maxSpeed = 700.0f;

    directionChangeDuration = sf::seconds(3.0f);
    directionChangeTimer = sf::Time::Zero;
    generateNewRandomDirection();
}

void Enemy::move(sf::Vector2f direction, sf::Time deltaTime)
{
    float dt = deltaTime.asSeconds();

    if (direction.x != 0 || direction.y != 0)
    {
        velocity += direction * acceleration * dt;
    }

    // Clamp enemy velocity to max speed
    float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (speed > maxSpeed)
    {
        velocity = (velocity / speed) * maxSpeed;
    }

    enemySprite.move(velocity * dt);
}

void Enemy::update(sf::Vector2u windowSize, sf::Time deltaTime)
{
	updateRandomMovement(deltaTime);
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

void Enemy::generateNewRandomDirection()
{
    float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * 3.14159265f;
    randomDirection = sf::Vector2f(cos(angle), sin(angle));
    directionChangeDuration = sf::seconds(2.5f);
    directionChangeTimer = sf::Time::Zero;
}

void Enemy::updateRandomMovement(sf::Time deltaTime)
{
    directionChangeTimer += deltaTime;

    // change direction periodically
    if (directionChangeTimer >= directionChangeDuration)
    {
        generateNewRandomDirection();
    }

    move(randomDirection, deltaTime);


    if (randomDirection.x != 0 || randomDirection.y != 0)
    {
        sf::Angle targetAngle = sf::radians(std::atan2(randomDirection.y, randomDirection.x)) + sf::degrees(90.0f);
        enemySprite.setRotation(targetAngle);
    }
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
