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

// Formation positions based on finger four formation
class Formation : public SteeringBehaviour
{
public:
    
    enum class Position {
        LEADER,     // User controlled
        LEFT_WING,
        RIGHT_WING,
        TAIL
    };

    // Formation behavior constants
	// Made these static constexpr to avoid multiple definitions error
    static constexpr float DEFAULT_MAX_ACCELERATION = 1200.0f;
    static constexpr float DEFAULT_MAX_SPEED = 600.0f;
    static constexpr float DEFAULT_SLOWING_RADIUS = 120.0f;
    static constexpr float DEFAULT_TARGET_RADIUS = 25.0f;
    static constexpr float DEFAULT_LEFT_WING_OFFSET = 120.0f;
    static constexpr float DEFAULT_RIGHT_WING_OFFSET = 120.0f;
    static constexpr float DEFAULT_TAIL_OFFSET = 180.0f;

    Formation(const Entity* leader, Position formationPosition, 
              float maxAcceleration = DEFAULT_MAX_ACCELERATION, 
              float maxSpeed = DEFAULT_MAX_SPEED, 
              float slowingRadius = DEFAULT_SLOWING_RADIUS, 
              float targetRadius = DEFAULT_TARGET_RADIUS,
              float offsetDistance = DEFAULT_LEFT_WING_OFFSET);

    SteeringOutput getSteering(const Entity& entity, sf::Time deltaTime) override;
    Position getPosition() const { return formationPosition; }

private:
    const Entity* leader;
    Position formationPosition;
    float offsetDistance; // Distance from leader

    // Arrive behaviour variables
    float maxAcceleration;
    float maxSpeed;
    float slowingRadius;
    float targetRadius;

    sf::Vector2f getFormationOffset() const;
    sf::Vector2f calculateTargetPosition() const;
    sf::Vector2f rotateVector(const sf::Vector2f& vector, float angleDegrees) const;
};
