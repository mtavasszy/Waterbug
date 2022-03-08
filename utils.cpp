#include "utils.h"

#include <SFML/Graphics.hpp>
#include "Vec2.h"

sf::Vector2f Utils::toSFVec(const Vec2f& v)
{
	return sf::Vector2f(v.x, v.y);
}

float Utils::getClockWiseAngle(const Vec2f& u, const Vec2f& v)
{
	if (u == v)
		return 360.f;
	float dot = u.x * v.x + u.y * v.y;
	float det = u.x * v.y - u.y * v.x;
	float angleRad = atan2(det, dot);
	float angleDeg = angleRad / (float(M_PI) / 180.f);
	if (angleDeg < 0)
		angleDeg += 360.f;

	return angleDeg;
}

// https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
bool Utils::isIntersect(const Vec2f& p1, const Vec2f& q1, const Vec2f& p2, const Vec2f& q2) {
	// Find the four orientations needed for general and special cases
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases
	// p1, q1 and p2 are collinear and p2 lies on segment p1q1
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	// p1, q1 and q2 are collinear and q2 lies on segment p1q1
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are collinear and p1 lies on segment p2q2
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are collinear and q1 lies on segment p2q2
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases
}

int Utils::orientation(const Vec2f& p, const Vec2f& q, const Vec2f& r)
{
	float val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
	if (val == 0) return 0;  // collinear
	return (val > 0) ? 1 : 2; // clock or counterclock wise
}

bool Utils::onSegment(const Vec2f& p, const Vec2f& q, const Vec2f& r)
{
	if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
		q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
		return true;

	return false;
}

// https://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
float Utils::closestLinePoint(Vec2f v, Vec2f w, Vec2f p) {
	// Return minimum distance between line segment vw and point p
	const float l2 = Vec2f::squaredDistance(v, w);  // i.e. |w-v|^2 -  avoid a sqrt
	if (l2 == 0.0) return Vec2f::distance(p, v);   // v == w case
	// Consider the line extending the segment, parameterized as v + t (w - v).
	// We find projection of point p onto the line. 
	// It falls where t = [(p-v) . (w-v)] / |w-v|^2
	// We clamp t from [0,1] to handle points outside the segment vw.
	const float t = std::max(0.f, std::min(1.f, Vec2f::dot(p - v, w - v) / l2));
	return t;
}

float Utils::mutateFloat(float val, int flipBit)
{
	uint8_t val_int = uint8_t((val * 255.f)+0.5f);
	val_int ^= uint8_t(1) << flipBit;

	return float(val_int) / 255.f;
}
