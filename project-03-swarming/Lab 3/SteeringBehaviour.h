#pragma once
#include "SteeringOutput.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Entity; // Forward declaration to avoid circular dependency
class NPC;    // Forward declaration for NPC class

class SteeringBehaviour
{
public:
    virtual ~SteeringBehaviour() = default;
    virtual SteeringOutput getSteering(const Entity& entity, sf::Time deltaTime) = 0;
};

class Seek : public SteeringBehaviour
{
public:
    Seek(const Entity* target, float maxAcceleration = 1000.0f);
    SteeringOutput getSteering(const Entity& entity, sf::Time deltaTime) override;

private:
    const Entity* target;
    float maxAcceleration;
};

class Wander : public SteeringBehaviour
{
public:
	Wander(float maxAcceleration = 300.0f, float changeInterval = 3.0f);
	SteeringOutput getSteering(const Entity& entity, sf::Time deltaTime) override;

private:
    float maxAcceleration;
    float changeInterval;
    sf::Vector2f direction;
    sf::Time timer;

    void generateNewDirection();
};

class Arrive : public SteeringBehaviour
{
public:
    Arrive(const Entity* target, float maxAcceleration, float maxSpeed, float slowingRadius, float targetRadius);
    SteeringOutput getSteering(const Entity& entity, sf::Time deltaTime) override;


private:
    const Entity* target;
    float maxAcceleration;
    float slowingRadius;    // Distance at which to start slowing down
	float targetRadius;     // Enemy will stop when within this distance
    float maxSpeed;
};

class Pursue : public SteeringBehaviour
{
public:
    Pursue(const Entity* target, float maxAcceleration = 1000.0f, float maxPrediction = 2.0f);
    SteeringOutput getSteering(const Entity& entity, sf::Time deltaTime) override;

private:
    const Entity* target;
    float maxAcceleration;
    float maxPredictionTime;
};

class LennardJonesSwarm : public SteeringBehaviour
{
public:
    LennardJonesSwarm(
        const std::vector<NPC*>* swarm,
        float attractionConstant = 100.0f, // attraction constant
		float repulsionConstant = 500.0f, // repulsion constant
		float attractionExponent = 1.0f,      // attraction exponent
		float repulsionExponent = 2.0f,      // repulsion exponent
        float maxAcceleration = 800.0f
        );

	SteeringOutput getSteering(const Entity& entity, sf::Time deltaTime) override;

private:
    const std::vector<NPC*>* swarm;
    float attractionConstant; 
    float repulsionConstant; 
    float attractionExponent; 
    float repulsionExponent; 
	float maxAcceleration;

    sf::Vector2f calculateLJForce(const sf::Vector2f& pos1, const sf::Vector2f& pos2) const;
};