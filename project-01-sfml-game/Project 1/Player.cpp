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

    // center the origin
	sf::Vector2u textureSize = playerTexture.getSize();
	playerSprite.setOrigin(sf::Vector2f(textureSize.x / 2.0f, textureSize.y / 2.0f));

	velocity = sf::Vector2f(0.0f, 0.0f);
	acceleration = 3500.0f; 
	deceleration = 1200.0f; 
	maxSpeed = 700.0f;
	rotationSpeed = 180.0f;
}

void Player::handleInput(sf::Time deltaTime)
{
	float dt = deltaTime.asSeconds();
    sf::Vector2f inputDirection(0.0f, 0.0f); // Reinitialized each frame for fresh inputs

    // Rotation logic
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        playerSprite.rotate(sf::degrees(-rotationSpeed * dt));
	}
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        playerSprite.rotate(sf::degrees(rotationSpeed * dt));
    }

    // Handle movement based on current rotation
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        inputDirection.y = -1.0f;  
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        inputDirection.y = 1.0f;

	// Need to convert inputDirection based on current rotation, otherwise movement is always up/down
    if (inputDirection.y != 0)
    {
        sf::Angle rotation = playerSprite.getRotation();
		sf::Angle adjustedRotation = rotation + sf::degrees(90.0f); // accounting for SFML sprite default orientation

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
        // acceleration applied upon input
		velocity += direction * acceleration * dt;
    }
    else
    {
        // linear deceleration upon no input
		float currentSpeed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        if (currentSpeed > 0)
        {
            float decelerationAmount = deceleration * dt;
            float newSpeed = std::max(0.0f, currentSpeed - decelerationAmount); // prevent negative speed

			// adjust speed while maintaining direction
            velocity = (velocity / currentSpeed) * newSpeed;
		}
    }

	// clamp velocity to max speed
    float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (speed > maxSpeed)
    {
        velocity = (velocity / speed) * maxSpeed;
	}

	playerSprite.move(velocity * dt);  
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

	if (bounds.position.x + bounds.size.x < 0) // off the left side of the screen
    {
        playerSprite.setPosition(sf::Vector2f(windowSize.x + bounds.size.x/2, position.y));
    }
	else if (bounds.position.x > windowSize.x) // off the right side of the screen
    {
        playerSprite.setPosition(sf::Vector2f(-bounds.size.x/2, position.y));
    }

	if (bounds.position.y + bounds.size.y < 0) // off the top of the screen
    {
        playerSprite.setPosition(sf::Vector2f(position.x, windowSize.y + bounds.size.y/2));
    }
	else if (bounds.position.y > windowSize.y) // off the bottom of the screen
    {
        playerSprite.setPosition(sf::Vector2f(position.x, -bounds.size.y/2));
    }
}
