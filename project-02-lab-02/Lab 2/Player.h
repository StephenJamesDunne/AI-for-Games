#pragma once

#include <SFML/Graphics.hpp>

class Player
{
public:
	Player(const std::string& textureFile);
	void handleInput(sf::Time deltaTime);
	void move(sf::Vector2f direction, sf::Time deltaTime);
	void update(sf::Vector2u windowSize);
	void draw(sf::RenderWindow& window);
	sf::Vector2f getPosition() const;


private:
	sf::Texture playerTexture;
	sf::Sprite playerSprite{ playerTexture };
	sf::Vector2f velocity;
	float acceleration;
	float deceleration;
	float maxSpeed;
	float rotationSpeed;
	void wrapAroundScreen(sf::Vector2u windowSize);
};
