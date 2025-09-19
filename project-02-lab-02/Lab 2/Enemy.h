#pragma once

#include <SFML/Graphics.hpp>

class Enemy
{
public:
	Enemy(const std::string& textureFile);
	void move(sf::Vector2f direction, sf::Time deltaTime);
	void update(sf::Vector2u windowSize, sf::Time deltaTime);
	void draw(sf::RenderWindow& window);
	sf::Vector2f getPosition() const;

private:
	sf::Texture enemyTexture;
	sf::Sprite enemySprite{ enemyTexture };
	sf::Vector2f velocity;
	float acceleration;
	float deceleration;
	float maxSpeed;

	sf::Vector2f randomDirection;
	sf::Time directionChangeTimer;
	sf::Time directionChangeDuration;

	void wrapAroundScreen(sf::Vector2u windowSize);
	void updateRandomMovement(sf::Time deltaTime);
	void generateNewRandomDirection();
};
