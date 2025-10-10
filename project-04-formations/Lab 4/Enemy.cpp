#include "Enemy.h"
#include "Math.h"

Enemy::Enemy(const std::string &textureFile, std::unique_ptr<SteeringBehaviour> behaviour, float customMaxSpeed) : Entity(),
                                                                                                                   steeringBehaviour(std::move(behaviour))
{
    // Enemy specific movement values
    acceleration = 2500.0f;
    deceleration = 800.0f;
    maxSpeed = customMaxSpeed;

    // Vision cone params
    visionRange = 200.0f;
    visionAngle = 30.0f; // total angle of vision cone in degrees

    if (loadTexture(textureFile))
    {
        setPosition(sf::Vector2f(1200.0f, 360.0f));
        centerOrigin();
    }

    // createVisionCone();
}

void Enemy::move(sf::Vector2f direction, sf::Time deltaTime)
{
    float dt = deltaTime.asSeconds();

    velocity += direction * dt;

    float speed = MathUtils::vectorLength(velocity);

    if (speed > maxSpeed)
    {
        velocity = MathUtils::normalize(velocity) * maxSpeed;
    }

    sprite.move(velocity * dt);

    if (speed > 0.1f)
    {
        sf::Vector2f normalizedVelocity = MathUtils::normalize(velocity);
        sf::Angle targetAngle = sf::radians(std::atan2(normalizedVelocity.y, normalizedVelocity.x)) + sf::degrees(90.0f);
        getSprite().setRotation(targetAngle);
    }
}

void Enemy::update(sf::Vector2u windowSize, sf::Time deltaTime)
{
    if (steeringBehaviour)
    {
        SteeringOutput steering = steeringBehaviour->getSteering(*this, deltaTime);
        move(steering.linear, deltaTime);

        // for angular movement if it is applied in the future
        if (steering.angular != 0.0f)
        {
            sprite.rotate(sf::degrees(steering.angular * deltaTime.asSeconds()));
        }
    }

    // updateVisionCone();
    wrapAroundScreen(windowSize);
}

void Enemy::draw(sf::RenderWindow &window)
{
    window.draw(visionCone);

    Entity::draw(window);
}

void Enemy::createVisionCone()
{
    visionCone.setPointCount(3);
    visionCone.setPoint(0, sf::Vector2f(0.0f, 0.0f));
    visionCone.setPoint(1, sf::Vector2f(-60, -visionRange));
    visionCone.setPoint(2, sf::Vector2f(60, -visionRange));
    visionCone.setFillColor(sf::Color(255, 255, 0, 100)); // Semi-transparent blue
    visionCone.setOutlineColor(sf::Color::Yellow);
}

void Enemy::updateVisionCone()
{
    visionCone.setPosition(getPosition());
    visionCone.setRotation(getSprite().getRotation());
}

bool Enemy::canSeePlayer(const sf::Sprite &playerSprite)
{
    // Function adjusted to take the player's sprite, instead of a single point in its position, for accuracy
    sf::FloatRect playerBounds = playerSprite.getGlobalBounds();

    sf::Vector2f playerCenter(
        playerBounds.position.x + playerBounds.size.x * 0.5f,
        playerBounds.position.y + playerBounds.size.y * 0.5f);

    sf::Vector2f toPlayer = playerCenter - getPosition();
    float distanceToPlayer = MathUtils::vectorLength(toPlayer);

    // Not in range, so ignore checks below this until this is true
    if (distanceToPlayer > visionRange || distanceToPlayer < 1.0f)
        return false;

    toPlayer = MathUtils::normalize(toPlayer);

    float enemyAngle = getSprite().getRotation().asRadians() - MathUtils::PI / 2.0f;
    sf::Vector2f enemyDirection(std::cos(enemyAngle), std::sin(enemyAngle));

    // Dot product to check against vision cone
    float dotProduct = MathUtils::dotProduct(toPlayer, enemyDirection);
    float angleThreshold = std::cos(MathUtils::toRadians(visionAngle) / 2.0f);

    return dotProduct >= angleThreshold;
}

void Enemy::setSteeringBehaviour(std::unique_ptr<SteeringBehaviour> behaviour)
{
    steeringBehaviour = std::move(behaviour);
}

std::string Enemy::getBehaviourName() const
{
    if (!steeringBehaviour)
        return "STOPPED";
    else if (dynamic_cast<Seek *>(steeringBehaviour.get()))
        return "Seek";
    else if (dynamic_cast<Wander *>(steeringBehaviour.get()))
        return "Wander";
    else if (dynamic_cast<Arrive *>(steeringBehaviour.get()))
        return "Arrive";
    else if (dynamic_cast<Pursue *>(steeringBehaviour.get()))
        return "Pursue";
    else if (Formation *formation = dynamic_cast<Formation *>(steeringBehaviour.get()))
    {
        switch (formation->getPosition())
        {
        case Formation::Position::LEFT_WING:
            return "Left Wing";
        case Formation::Position::RIGHT_WING:
            return "Right Wing";
        case Formation::Position::TAIL:
            return "Tail";
        case Formation::Position::LEADER:
            return "Leader";
        default:
            return "Unknown";
        }
    }
    else
        return "None";
}