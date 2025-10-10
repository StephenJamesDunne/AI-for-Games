#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

namespace MathUtils {
	const float PI = 3.14159265358979323846f;

	float toDegrees(float radians);

	float toRadians(float degrees);

	float vectorLength(const sf::Vector2f& vec);

	sf::Vector2f normalize(const sf::Vector2f& vec);

	float dotProduct(const sf::Vector2f& a, const sf::Vector2f& b);

	float angleBetween(const sf::Vector2f& a, const sf::Vector2f& b);
}