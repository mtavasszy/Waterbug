#ifndef NODE_H_ 
#define NODE_H_

#include <SFML/Graphics.hpp>
#include "Vec2.h"

class Muscle;
class Node {

public:
	Node(Vec2f position);

	void update(float dt);
	void draw(sf::RenderWindow& window);

	// physics information
	Vec2f m_position;
	Vec2f m_velocity;
	Vec2f m_force;
	float m_mass = 100.f;

	// drawing information
	float m_nodeRadius = 12.f;
	sf::CircleShape m_circle;
	sf::Color m_fillColor = sf::Color::Red;
};

#endif  // NODE_H_
