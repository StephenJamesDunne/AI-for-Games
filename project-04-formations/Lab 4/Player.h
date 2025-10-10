#pragma once
#include "Entity.h"

class Player : public Entity
{
public:
	Player(const std::string& textureFile);
	void move(sf::Vector2f direction, sf::Time deltaTime) override;
	void handleInput(sf::Time deltaTime);
	void update(sf::Vector2u windowSize, sf::Time deltaTime) override;

private:
	
};
