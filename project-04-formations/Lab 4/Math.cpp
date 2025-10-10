#include "Math.h"
#include <cmath>

namespace MathUtils {

	float toDegrees(float radians)
	{
		return radians * (180.0f / PI);
	}

	float toRadians(float degrees)
	{
		return degrees * (PI / 180.0f);
	}

	float vectorLength(const sf::Vector2f& vec)
	{
		return std::sqrt(vec.x * vec.x + vec.y * vec.y);
	}

	sf::Vector2f normalize(const sf::Vector2f& vec)
	{
		float len = vectorLength(vec);
		if (len != 0.f) return vec / len;
		return sf::Vector2f(0.f, 0.f);
	}

	float dotProduct(const sf::Vector2f& a, const sf::Vector2f& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	float angleBetween(const sf::Vector2f& a, const sf::Vector2f& b)
	{
		float dot = dotProduct(normalize(a), normalize(b));
		return std::acos(dot);
	}
}