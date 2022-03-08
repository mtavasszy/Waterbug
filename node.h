#ifndef NODE_H_ 
#define NODE_H_

#include <SFML/Graphics.hpp>
#include "Vec2.h"

class Muscle;
class Creature;

class Node {

public:
	Node(Vec2f position);
	Node(const Node* n);

	void ResetForces();
	void ApplyDrag(Creature* parent, int nodeId);
	//void ApplyPushBack(Muscle* m);
	void CorrectCollisions(Creature* parent, int nodeId);
	void ApplyForces();
	void Draw(sf::RenderWindow& window, Vec2f camPos);

	// physics information
	Vec2f m_position;
	Vec2f m_velocity;
	Vec2f m_internalForce;
	//Vec2f m_reactionForce;
	Vec2f m_dragForce;
	float m_mass;

	std::vector<int> m_connectedNodes;

	// drawing information
	float m_nodeRadius = 12.f;
	sf::CircleShape m_circle;
	sf::Color m_fillColor = sf::Color::Red;
};

#endif  // NODE_H_
