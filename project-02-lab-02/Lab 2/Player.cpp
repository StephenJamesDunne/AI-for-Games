#include "Player.h"
#include "Math.h"

Player::Player(const std::string& textureFile) : Entity()
{
    acceleration = 4000.0f;
    deceleration = 1500.0f;
    maxSpeed = 500.0f;
    rotationSpeed = 180.0f;

    if (loadTexture(textureFile))
    {
        setPosition(sf::Vector2f(100.0f, 200.0f));
        centerOrigin();
    }
}

void Player::handleInput(sf::Time deltaTime)
{
    float dt = deltaTime.asSeconds();
    sf::Vector2f inputDirection(0.0f, 0.0f);

    // Rotation logic
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        sprite.rotate(sf::degrees(-rotationSpeed * dt));
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        sprite.rotate(sf::degrees(rotationSpeed * dt));
    }

    // Handle movement based on current rotation
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        inputDirection.y = -1.0f;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        inputDirection.y = 1.0f;

    // Convert inputDirection based on current rotation
    if (inputDirection.y != 0)
    {
        sf::Angle rotation = sprite.getRotation();
        sf::Angle adjustedRotation = rotation + sf::degrees(90.0f);
        float cos_r = std::cos(adjustedRotation.asRadians());
        float sin_r = std::sin(adjustedRotation.asRadians());
        inputDirection = sf::Vector2f(
            inputDirection.y * cos_r,
            inputDirection.y * sin_r
        );
    }

    move(inputDirection, deltaTime);
}

void Player::move(sf::Vector2f direction, sf::Time deltaTime)
{
    float dt = deltaTime.asSeconds();

    if (direction.x != 0 || direction.y != 0)
    {
        // Direct acceleration based on input
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

    // Clamp to max speed and apply to sprite
    float speed = MathUtils::vectorLength(velocity);
    if (speed > maxSpeed)
    {
        velocity = MathUtils::normalize(velocity) * maxSpeed;
    }
    
    getSprite().move(velocity * deltaTime.asSeconds());
}

void Player::update(sf::Vector2u windowSize, sf::Time deltaTime)
{
    wrapAroundScreen(windowSize);
}