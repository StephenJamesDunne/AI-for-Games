#include "NPC.h"

NPC::NPC(const std::string& textureFile) : Entity()
{
	
}

void NPC::move(sf::Vector2f direction, sf::Time deltaTime)
{
	// Swarming behavior to be implemented
}

void NPC::update(sf::Vector2u windowSize, sf::Time deltaTime)
{
	// Calls to move() swarming function to be implemented
}

