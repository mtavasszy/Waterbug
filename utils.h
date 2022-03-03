#ifndef UTILS_H_ 
#define UTILS_H_

#include <SFML/Graphics.hpp>
#include "Vec2.h"

class Utils {
public:
	static sf::Vector2f toSFVec(const Vec2f& v);
	static float getClockWiseAngle(const Vec2f& u, const Vec2f& v);
	static bool isIntersect(const Vec2f& p1, const Vec2f& q1, const Vec2f& p2, const Vec2f& q2);
	static int orientation(const Vec2f& p, const Vec2f& q, const Vec2f& r);
	static bool onSegment(const Vec2f& p, const Vec2f& q, const Vec2f& r);
	static float closestLinePoint(Vec2f v, Vec2f w, Vec2f p);

private:
	Utils() {}
};

#endif  // UTILS_H_