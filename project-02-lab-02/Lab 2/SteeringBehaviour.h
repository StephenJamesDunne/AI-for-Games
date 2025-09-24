#pragma once
#include "SteeringOutput.h"
#include <SFML/Graphics.hpp>

class Entity; // Forward declaration to avoid circular dependency

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
    Arrive(const Entity* target, float maxAcceleration = 1200.0f, float slowingRadius = 200.0f, float targetRadius = 10.0f);
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
    float maxPrediction; // Maximum prediction time
};