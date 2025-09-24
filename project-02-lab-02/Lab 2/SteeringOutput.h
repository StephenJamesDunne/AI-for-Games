#pragma once
#include <SFML/Graphics.hpp>

struct SteeringOutput {
    sf::Vector2f linear;    // Linear acceleration
    float angular;          // Angular acceleration
};