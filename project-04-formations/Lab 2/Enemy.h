#pragma once
#include "Entity.h"
#include "SteeringBehaviour.h"
#include "SteeringOutput.h"
#include <memory>

class Enemy : public Entity
{
public:
    Enemy(const std::string& textureFile, std::unique_ptr<SteeringBehaviour> behaviour = nullptr, float customMaxSpeed = 300.0f);

    void move(sf::Vector2f direction, sf::Time deltaTime) override;
    void update(sf::Vector2u windowSize, sf::Time deltaTime) override;
    void draw(sf::RenderWindow& window) override; 

	// function for dynamically changing steering behaviour at runtime
	void setSteeringBehaviour(std::unique_ptr<SteeringBehaviour> behaviour);
    bool canSeePlayer(const sf::Sprite& playerSprite);
    void setVisionConeColor(sf::Color color) { visionCone.setFillColor(color); }

    std::string getBehaviourName() const;

private:
	std::unique_ptr<SteeringBehaviour> steeringBehaviour;

    sf::ConvexShape visionCone; // Vision cone shape for visualization
    float visionRange;
    float visionAngle;

    void createVisionCone();
    void updateVisionCone();
};