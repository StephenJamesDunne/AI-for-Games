#include "SteeringBehaviour.h"
#include "Entity.h"
#include "NPC.h"
#include "Math.h"
#include <iostream>
#include <cmath>

// Seek implementation
Seek::Seek(const Entity* target, float maxAcceleration) : 
    target(target), 
    maxAcceleration(maxAcceleration) {}

SteeringOutput Seek::getSteering(const Entity& entity, sf::Time deltaTime) 
{
    SteeringOutput steering;
    
    // Get direction to target
    sf::Vector2f direction = target->getPosition() - entity.getPosition();
    
    // Normalize and apply max acceleration
    steering.linear = MathUtils::normalize(direction) * maxAcceleration;
    steering.angular = 0.0f; // ignore angular for now
    
    return steering;
}


// Wander implementation
Wander::Wander(float maxAcceleration, float changeInterval) : 
    maxAcceleration(maxAcceleration), 
    changeInterval(changeInterval), 
    timer(sf::Time::Zero)
{
    generateNewDirection();
}

SteeringOutput Wander::getSteering(const Entity& entity, sf::Time deltaTime)
{
    timer += deltaTime;

    if (timer >= sf::seconds(changeInterval))
    {
        generateNewDirection();
        timer = sf::Time::Zero;
    }

    SteeringOutput steering;
    steering.linear = direction * maxAcceleration;
    steering.angular = 0.0f;
    return steering;
}

void Wander::generateNewDirection()
{
    float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * MathUtils::PI;
    direction = sf::Vector2f(std::cos(angle), std::sin(angle));
}


// Arrive implementation
Arrive::Arrive(const Entity* target, float maxAcceleration, float maxSpeed, float slowingRadius, float targetRadius) : 
    target(target),
    maxAcceleration(maxAcceleration), 
    slowingRadius(slowingRadius),
    targetRadius(targetRadius),
    maxSpeed(maxSpeed) {}

SteeringOutput Arrive::getSteering(const Entity& entity, sf::Time deltaTime)
{
    SteeringOutput steering;

	sf::Vector2f direction = target->getPosition() - entity.getPosition();
    float distance = MathUtils::vectorLength(direction);

    if (distance < targetRadius)
    {
        sf::Vector2f currentVelocity = entity.getVelocity();
		steering.linear = -currentVelocity * 10.0f;
        steering.angular = 0.0f;
        return steering;
	}

    float targetSpeed;
    if (distance > slowingRadius)
    {
        targetSpeed = maxSpeed;
    }
    else
    {
		float speedRatio = distance / slowingRadius;
		targetSpeed = maxSpeed * speedRatio * speedRatio;

        float minSpeed = maxSpeed * 0.2f;
		targetSpeed = std::max(targetSpeed, minSpeed);
	}

    sf::Vector2f targetVelocity = MathUtils::normalize(direction) * targetSpeed;

	sf::Vector2f currentVelocity = entity.getVelocity();

	steering.linear = (targetVelocity - currentVelocity);

	float steeringMagnitude = MathUtils::vectorLength(steering.linear);
    if (steeringMagnitude > maxAcceleration)
    {
        steering.linear = MathUtils::normalize(steering.linear) * maxAcceleration;
		
    }

	steering.angular = 0.0f;

	return steering;
}

// Pursue implementation
Pursue::Pursue(const Entity* target, float maxAcceleration, float maxPredictionTime) : 
    target(target), 
    maxAcceleration(maxAcceleration), 
	maxPredictionTime(maxPredictionTime) {
}

SteeringOutput Pursue::getSteering(const Entity& entity, sf::Time deltaTime) 
{
    SteeringOutput steering;
    
    sf::Vector2f toTarget = target->getPosition() - entity.getPosition();
    float distanceToTarget = MathUtils::vectorLength(toTarget);

    sf::Vector2f entityVelocity = entity.getVelocity();
    sf::Vector2f targetVelocity = target->getVelocity();
    float entitySpeed = MathUtils::vectorLength(entityVelocity);
    
    float predictionTime;

	// Calculate prediction time by comparing distance to pursuer's speed
    // DST: if the enemy is moving too slowly, use maxPrediction time
    // DST: if the enemy is moving too fast, reduce prediction time
    if (entitySpeed <= distanceToTarget / maxPredictionTime) 
    {
        predictionTime = maxPredictionTime;
    } 
    else 
    {
        predictionTime = distanceToTarget / entitySpeed;
	}
    
    // Calculate predicted target position
    sf::Vector2f predictedPosition = target->getPosition() + targetVelocity * predictionTime;

    // Seek to predicted position
    sf::Vector2f direction = predictedPosition - entity.getPosition();

    if (MathUtils::vectorLength(direction) < 0.1f)
    {
        steering.linear = sf::Vector2f(0.0f, 0.0f);
        steering.angular = 0.0f;
        return steering;
    }

    // Apply max acceleration toward predicted position
    steering.linear = MathUtils::normalize(direction) * maxAcceleration;
    steering.angular = 0.0f;

    return steering;
}

// Lennard-Jones implementation
LennardJonesSwarm::LennardJonesSwarm(
    const std::vector<NPC*>* swarm,
    float attractionConstant,
    float repulsionConstant,
    float attractionExponent,
    float repulsionExponent,
    float maxAcceleration) :
    swarm(swarm),
    attractionConstant(attractionConstant),
    repulsionConstant(repulsionConstant),
    attractionExponent(attractionExponent),
    repulsionExponent(repulsionExponent),
    maxAcceleration(maxAcceleration) {
}

SteeringOutput LennardJonesSwarm::getSteering(const Entity& entity, sf::Time deltaTime) 
{
    SteeringOutput steering;
    sf::Vector2f totalForce(0.0f, 0.0f);

    sf::Vector2f currentPos = entity.getPosition();

    for (const NPC* other : *swarm)
    {
        // skip self if targeted in loop
        if (&entity == other)
            continue;

        sf::Vector2f otherPos = other->getPosition();
        sf::Vector2f force = calculateLJForce(currentPos, otherPos);
        totalForce += force;
    }

    // Clamp total force to maxAcceleration
    float forceMagnitude = MathUtils::vectorLength(totalForce);
    if (forceMagnitude > maxAcceleration)
    {
        totalForce = MathUtils::normalize(totalForce) * maxAcceleration;
    }

    steering.linear = totalForce;
    steering.angular = 0.0f;

    return steering;
}

sf::Vector2f LennardJonesSwarm::calculateLJForce(const sf::Vector2f& pos1, const sf::Vector2f& pos2) const
{
    sf::Vector2f direction = pos1 - pos2;
    float distance = MathUtils::vectorLength(direction);

	const float minDistance = 0.5f; // to prevent division by zero
    if (distance < minDistance)
    {
		distance = minDistance;
    }

    // Calculate Lennard-Jones force
    float attractionMagnitude = -attractionConstant / std::pow(distance, attractionExponent);
    float repulsionMagnitude = repulsionConstant / std::pow(distance, repulsionExponent);
	float potential = attractionMagnitude + repulsionMagnitude;

    sf::Vector2f normalizedDirection = MathUtils::normalize(direction);

	return normalizedDirection * potential;
}