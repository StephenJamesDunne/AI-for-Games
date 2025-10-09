#include "NPC.h"
#include "Math.h"

NPC::NPC(const std::string& textureFile) : Entity()
{
	if (loadTexture(textureFile))
	{
		centerOrigin();
	}
}

void NPC::move(sf::Vector2f direction, sf::Time deltaTime)
{
    float dt = deltaTime.asSeconds();
	
    velocity += direction * dt;
    
    getSprite().move(velocity * dt);
}

void NPC::update(sf::Vector2u windowSize, sf::Time deltaTime)
{
    if (steeringBehavior != nullptr)
    {
        SteeringOutput steering = steeringBehavior->getSteering(*this, deltaTime);
        
        move(steering.linear, deltaTime);
    }

	wrapAroundScreen(windowSize);
}

void NPC::setSteeringBehavior(SteeringBehaviour* behavior)
{
	steeringBehavior = behavior;
}
