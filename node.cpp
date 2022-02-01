#define _USE_MATH_DEFINES

#include "node.h"
#include <SFML/Graphics.hpp>
#include <math.h>
#include "Vec2.h"


Node::Node() {
	initialize();
}

Node::Node(bool isMainNode)
{
	m_isMainNode = isMainNode;
	initialize();
}


Node::Node(float a0, float a1, float a0_l) {
	initialize();
	
	m_armAngle0 = a0;
	m_armAngle1 = a1;
	m_armAngle0_t = a0_l;

	m_armAnglePos = m_armAngle1;
}

void Node::initialize()
{
	m_circle = sf::CircleShape(m_nodeRadius);
	m_circle.setPointCount(20);
	m_circle.setFillColor(m_isMainNode ? sf::Color::Red : sf::Color::White);
	m_circle.setOutlineColor(sf::Color::Black);
	m_circle.setOutlineThickness(-2.f);

	m_clockStart = 0.f; // random?
	m_clock = m_clockStart;

	m_rotation = 0;
	m_rotationSpeed = 0;
	m_position = Vec2f(0.f, 0.f);
	m_velocity = Vec2f(0.f, 0.f);
	m_acceleration = Vec2f(0.f, 0.f);
	m_mass = 1;

	m_connectedNodes = std::vector<Node*>();

	for (int i = 0; i < 4; ++i)
		m_edgeVertices[i].color = sf::Color::White;
}

void Node::setParent(Node* p)
{
	m_parent = p;
	p->m_connectedNodes.push_back(this);
}
//
//void Node::updateCurrentAngle() {
//
//	float targetAngleDiff = armAngle1 - armAngle0;
//	float targetAcc = (40.f * targetAngleDiff);
//
//	if (clock < armAngle0_t) // a1 -> a0 accelerate
//		armAngleAcc = -targetAcc;
//	if (clock > armAngle0_t) // a0 -> a1 accelerate
//		armAngleAcc = targetAcc;
//
//	armAngleVel += armAngleAcc * deltaClock;
//	armAnglePos += armAngleVel * deltaClock;
//
//	if (armAnglePos < armAngle0) {
//		armAnglePos = armAngle0;
//		armAngleVel = 0;
//		armAngleAcc = 0;
//	}
//
//	if (armAnglePos > armAngle1) {
//		armAnglePos = armAngle1;
//		armAngleVel = 0;
//		armAngleAcc = 0;
//	}
//}
//
//void Node::simulateStep() {
//	if (parent != nullptr) { // exclude body node
//		updateCurrentAngle();
//
//		clock += deltaClock;
//		if (clock > 1.f) {
//			clock = 0;
//		}
//	}
//
//	for (int i = 0; i < appendages.size();i++) {
//		Node* n = &appendages[i];
//		n->simulateStep();
//	}
//}
//
inline float DegToRad(float Deg)
{
	return Deg / 180.f * float(M_PI);
}


void Node::update(float dt)
{
}

void Node::updatePosition()
{
	if (!m_isMainNode) { // exclude body node
		m_rotation = m_parent->m_rotation + m_armAnglePos;
		m_position = m_parent->m_position + Vec2f(std::cosf(DegToRad(m_rotation)), std::sinf(DegToRad(m_rotation))) * m_armLength;
	}
}

inline sf::Vector2f toSFVec(Vec2f v) {
	return sf::Vector2f(v.x, v.y);
}

void Node::drawEdge(sf::RenderWindow& window)
{
	if (!m_isMainNode) {
		Vec2f direction = m_position - m_parent->m_position;
		Vec2f unitDirection = direction.normalize();
		Vec2f unitPerpendicular(-unitDirection.y, unitDirection.x);

		Vec2f offset = (m_edgeThickness / 2.f) * unitPerpendicular;

		m_edgeVertices[0].position = toSFVec(m_position + offset);
		m_edgeVertices[1].position = toSFVec(m_parent->m_position + offset);
		m_edgeVertices[2].position = toSFVec(m_parent->m_position - offset);
		m_edgeVertices[3].position = toSFVec(m_position - offset);

		window.draw(m_edgeVertices, 4, sf::Quads);
	}
}

void Node::drawNode(sf::RenderWindow& window)
{
	m_circle.setPosition(toSFVec(m_position - m_circle.getRadius()));
	window.draw(m_circle);
}
