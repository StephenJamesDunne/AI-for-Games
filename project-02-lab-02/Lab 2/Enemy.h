#pragma once
#include "Entity.h"

class Enemy : public Entity
{
public:
    Enemy(const std::string& textureFile);

    void move(sf::Vector2f direction, sf::Time deltaTime) override;
    void update(sf::Vector2u windowSize, sf::Time deltaTime) override;

private:
    sf::Vector2f randomDirection;
    sf::Time directionChangeTimer;
    sf::Time directionChangeDuration;

    void updateRandomMovement(sf::Time deltaTime);
    void generateNewRandomDirection();
};