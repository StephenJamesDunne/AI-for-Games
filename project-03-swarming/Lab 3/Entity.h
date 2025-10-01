#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Entity
{
public:
    Entity();
    virtual ~Entity() = default;

	// functions used across both Player and Enemy
    virtual void move(sf::Vector2f direction, sf::Time deltaTime) = 0;
    virtual void update(sf::Vector2u windowSize, sf::Time deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window);
    sf::Vector2f getPosition() const;

	// getters for sprite to allow access in main for rotation
    sf::Sprite& getSprite() { return sprite; }
    const sf::Sprite& getSprite() const { return sprite; }
	sf::Vector2f getVelocity() const { return velocity; }

    void setPosition(sf::Vector2f position);

protected:
    // common values
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f velocity;
    float acceleration;
    float deceleration;
    float maxSpeed;
    float rotationSpeed;

    void wrapAroundScreen(sf::Vector2u windowSize);
    void centerOrigin();
	bool loadTexture(const std::string& textureFile);
};