#pragma once

#include <SFML/Graphics.hpp>

class Enemy
{
public:
	Enemy(const std::string& textureFile);
	void handleInput(sf::Time deltaTime);
	void move(sf::Vector2f direction, sf::Time deltaTime);
	void update(sf::Vector2u windowSize);
	void draw(sf::RenderWindow& window);
	sf::Vector2f getPosition() const;


private:
	sf::Texture enemyTexture;
	sf::Sprite enemySprite{ enemyTexture };
	float movementSpeed{ 400.0f };
	void wrapAroundScreen(sf::Vector2u windowSize);
};
