#include "muscle.h"
#include "node.h"
#include "Vec2.h"

Muscle::Muscle(Node* A, Node* B, float expandLength, float contractLength)
{
	// Connected nodes
	m_nodeA = A;
	m_nodeB = B;
	//m_nodeA->m_connectedMuscles.push_back(this);
	//m_nodeB->m_connectedMuscles.push_back(this);
	m_expandLength = expandLength;
	m_contractLength = contractLength;

	// Clock
	m_clock = m_clockStart;
	m_restLength = m_expandLength;

	// Draw
	for (int i = 0; i < 4; ++i)
		m_edgeVertices[i].color = sf::Color::White;
}

void Muscle::update(float dt)
{
	updateClock(dt);
	updateForces(dt);
}

void Muscle::updateClock(float dt)
{
	m_clock += m_clockSpeed * dt;
	if (m_clock >= 1) {
		m_clock = 0;
	}
	if (m_clock > m_contractTime) {
		m_restLength = m_contractLength;
	}
	else {
		m_restLength = m_expandLength;
	}
}

void Muscle::updateForces(float dt)
{
	Vec2f d_p = m_nodeB->m_position - m_nodeA->m_position;
	Vec2f d_v = m_nodeB->m_velocity - m_nodeA->m_velocity;
	float dist = d_p.getLength();
	d_p /= dist; // normalize

	float F_s = (dist - m_restLength) * m_stiffness;
	float F_d = Vec2f::dot(d_p, d_v) * m_damping;

	float F_t = F_s + F_d;

	m_nodeA->m_force += F_t * d_p;
	m_nodeB->m_force += F_t * -d_p;
}

inline sf::Vector2f toSFVec(Vec2f v) {
	return sf::Vector2f(v.x, v.y);
}

void Muscle::draw(sf::RenderWindow& window)
{
	Vec2f direction = m_nodeA->m_position - m_nodeB->m_position;
	Vec2f unitDirection = direction.normalize();
	Vec2f unitPerpendicular(-unitDirection.y, unitDirection.x);

	Vec2f offset = (m_edgeThickness / 2.f) * unitPerpendicular;

	m_edgeVertices[0].position = toSFVec(m_nodeA->m_position + offset);
	m_edgeVertices[1].position = toSFVec(m_nodeB->m_position + offset);
	m_edgeVertices[2].position = toSFVec(m_nodeB->m_position - offset);
	m_edgeVertices[3].position = toSFVec(m_nodeA->m_position - offset);

	window.draw(m_edgeVertices, 4, sf::Quads);
}
