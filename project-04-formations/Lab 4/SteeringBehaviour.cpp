#include "SteeringBehaviour.h"
#include "Entity.h"
#include "Math.h"
#include <iostream>

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

Formation::Formation(const Entity* leader, Position formationPosition, 
                    float maxAcceleration, float maxSpeed, float slowingRadius, float targetRadius,
                    float offsetDistance) :
    leader(leader),
    formationPosition(formationPosition),
    offsetDistance(offsetDistance),
    maxAcceleration(maxAcceleration),
    maxSpeed(maxSpeed),
    slowingRadius(slowingRadius),
    targetRadius(targetRadius) {}

SteeringOutput Formation::getSteering(const Entity& entity, sf::Time deltaTime)
{
    SteeringOutput steering;

    // Get the desired formation position
    sf::Vector2f targetPosition = calculateTargetPosition();
    
    // Use Arrive behavior to move into formation position
    sf::Vector2f direction = targetPosition - entity.getPosition();
    float distance = MathUtils::vectorLength(direction);

    // Aggressive braking when within target radius of the formation position
    if (distance < targetRadius)
    {
        sf::Vector2f currentVelocity = entity.getVelocity();
        steering.linear = -currentVelocity * 10.0f;
        steering.angular = 0.0f;
        return steering;
    }

    // Calculate target speed based on distance to target position
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

    // Clamp to max acceleration
    float steeringMagnitude = MathUtils::vectorLength(steering.linear);
    if (steeringMagnitude > maxAcceleration)
    {
        steering.linear = MathUtils::normalize(steering.linear) * maxAcceleration;
    }

    steering.angular = 0.0f;
    return steering;
}

// Function to apply both the leader's position and orientation to get the target formation position
sf::Vector2f Formation::calculateTargetPosition() const
{
    sf::Vector2f baseOffset = getFormationOffset();

	float leaderRotation = leader->getSprite().getRotation().asDegrees();
    sf::Vector2f rotatedOffset = rotateVector(baseOffset, leaderRotation);

    return leader->getPosition() + rotatedOffset;
}

sf::Vector2f Formation::getFormationOffset() const
{
    // Align offsets based on formation position
    switch (formationPosition)
    {
        case Position::LEFT_WING:
            return sf::Vector2f(-offsetDistance, offsetDistance * 0.7f);
        case Position::RIGHT_WING:
            return sf::Vector2f(offsetDistance, offsetDistance * 0.7f);
        case Position::TAIL:
            return sf::Vector2f(offsetDistance * 1.5f, offsetDistance * 1.4f);
        default:
            return sf::Vector2f(0.0f, 0.0f);
    }
}

// Function to rotate a vector by a given angle in degrees
sf::Vector2f Formation::rotateVector(const sf::Vector2f& vector, float angleDegrees) const
{
    float angleRadians = angleDegrees * (MathUtils::PI / 180.0f);
    float cosAngle = std::cos(angleRadians);
    float sinAngle = std::sin(angleRadians);

    return sf::Vector2f(
        vector.x * cosAngle - vector.y * sinAngle,
        vector.x * sinAngle + vector.y * cosAngle
    );
}