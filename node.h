#ifndef NODE_H_ 
#define NODE_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include "Vec2.h"

class Node {

public:
	Node();
	Node(bool isMainNode);
	Node(float a0, float a1, float a0_l);
	void initialize();
	void setParent(Node* p);

	void update(float dt);
	void updatePosition();
	void drawEdge(sf::RenderWindow& window);
	void drawNode(sf::RenderWindow& window);

	bool m_isMainNode = false;
	Node* m_parent;
	std::vector<Node*> m_connectedNodes;

	// physics information
	float m_armLength = 40.f;
	float m_mass = 1.f;
	float m_rotation;
	float m_rotationSpeed;
	Vec2f m_position;
	Vec2f m_velocity;
	Vec2f m_acceleration;

	// muscle information
	float m_clock; // runs from 0 to 1
	float m_clockStart;
	float m_deltaClock = 0.02f;

	float m_armAngleVel = 1.f;
	float m_armAnglePos;
	float m_armAngle0, m_armAngle1;
	float m_armAngle0_t;

	// drawing information
	float m_nodeRadius = 12.f;
	sf::CircleShape m_circle;
	float m_edgeThickness = 4.f;
	sf::Vertex m_edgeVertices[4];

};

#endif  // NODE_H_
