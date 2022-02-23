#ifndef UTILS_H_ 
#define UTILS_H_

#include <SFML/Graphics.hpp>
#include "Vec2.h"

inline sf::Vector2f toSFVec(const Vec2f v) {
	return sf::Vector2f(v.x, v.y);
}

#endif  // UTILS_H_