#ifndef NODE_H_ 
#define NODE_H_

#include <SFML/Graphics.hpp>
#include "Vec2.h"

class Node {

public:
	Node(Vec2f position);
	Node(const Node* n);

	void ApplyForces(float dt);
	void Draw(sf::RenderWindow& window, Vec2f camPos);

	// physics information
	Vec2f m_position;
	Vec2f m_velocity;
	Vec2f m_internalforce;
	Vec2f m_externalForce;
	float m_mass = 100.f;

	std::vector<int> m_connectedNodes;

	// drawing information
	float m_nodeRadius = 12.f;
	sf::CircleShape m_circle;
	sf::Color m_fillColor = sf::Color::Red;
};

#endif  // NODE_H_
