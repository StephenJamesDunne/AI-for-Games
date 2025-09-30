#pragma once
#include "Entity.h"

class NPC : public Entity
{
public:
	NPC(const std::string& textureFile);
	void move(sf::Vector2f direction, sf::Time deltaTime) override;
	void update(sf::Vector2u windowSize, sf::Time deltaTime) override;

private:

};