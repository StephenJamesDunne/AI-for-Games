#include "Enemy.h"
#include "Math.h"

Enemy::Enemy(const std::string& textureFile) : Entity()
{
    // Enemy specific movement values
    acceleration = 2500.0f;
    deceleration = 800.0f; 
    maxSpeed = 300.0f;
    directionChangeDuration = (sf::seconds(2.0f + static_cast<float>(rand()) / RAND_MAX * 3.0f));
    directionChangeTimer = sf::Time::Zero;
    
    if (loadTexture(textureFile))
    {
        setPosition(sf::Vector2f(700.0f, 300.0f));
        centerOrigin();
    }

    generateNewRandomDirection();
}

void Enemy::move(sf::Vector2f direction, sf::Time deltaTime)
{
    float dt = deltaTime.asSeconds();

    if (direction.x != 0 || direction.y != 0)
    {
        // Apply acceleration in the given direction
        velocity += direction * acceleration * dt;
    }
    else
    {
        float currentSpeed = MathUtils::vectorLength(velocity);
        if (currentSpeed > 0)
        {
            float decelerationAmount = deceleration * deltaTime.asSeconds();
            float newSpeed = std::max(0.0f, currentSpeed - decelerationAmount);
            velocity = MathUtils::normalize(velocity) * newSpeed;
        }
    }

    float speed = MathUtils::vectorLength(velocity);
    if (speed > maxSpeed)
    {
        velocity = MathUtils::normalize(velocity) * maxSpeed;
    }

    sprite.move(velocity * deltaTime.asSeconds());
}

void Enemy::update(sf::Vector2u windowSize, sf::Time deltaTime)
{
    updateRandomMovement(deltaTime);
    wrapAroundScreen(windowSize);
}

void Enemy::generateNewRandomDirection()
{
    float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * MathUtils::PI;
    randomDirection = sf::Vector2f(cos(angle), sin(angle));
    directionChangeDuration = sf::seconds(2.0f + static_cast<float>(rand()) / RAND_MAX * 3.0f);
    directionChangeTimer = sf::Time::Zero;
}

void Enemy::updateRandomMovement(sf::Time deltaTime)
{
    directionChangeTimer += deltaTime;

    if (directionChangeTimer >= directionChangeDuration)
    {
        generateNewRandomDirection();
    }

    move(randomDirection, deltaTime);

    if (randomDirection.x != 0 || randomDirection.y != 0)
    {
        sf::Angle targetAngle = sf::radians(std::atan2(randomDirection.y, randomDirection.x)) + sf::degrees(90.0f);
        getSprite().setRotation(targetAngle);
    }
}