#include "muscle.h"
#include "node.h"
#include "Vec2.h"
#include "config.h"

Muscle::Muscle(int A_i, int B_i, Node* A, Node* B, float expandLength, float contractLength, float clockStart, float contractTime)
{
	// connected nodes
	m_nodeAIndex = A_i;
	m_nodeBIndex = B_i;
	m_nodeA = A;
	m_nodeB = B;
	m_expandLength = expandLength;
	m_contractLength = contractLength;

	// clock
	m_clockStart = clockStart;
	m_clock = m_clockStart;
	m_restLength = m_expandLength;
	m_contractTime = contractTime;

	// draw
	for (int i = 0; i < 4; ++i)
		m_edgeVertices[i].color = sf::Color::White;
}

Muscle::Muscle(const Muscle* m)
{
	m_nodeAIndex = m->m_nodeAIndex;
	m_nodeBIndex = m->m_nodeBIndex;

	// spring model info
	m_expandLength = m->m_expandLength;
	m_contractLength = m->m_contractLength;
	m_restLength = m->m_restLength;
	m_stiffness = m->m_stiffness;
	m_damping = m->m_damping;

	// movement
	m_clock = m->m_clock; // runs from 0 to 1
	m_clockStart = m->m_clockStart;
	m_clockSpeed = m->m_clockSpeed;
	m_contractTime = m->m_contractTime;

	// drawing information
	m_expandThickness = m->m_expandThickness;
	m_contractThickness = m->m_contractThickness;
	m_edgeThickness = m->m_edgeThickness;
	for (int i = 0; i < 4; i++) {
		m_edgeVertices[i] = m->m_edgeVertices[i];
	}
}

void Muscle::UpdateClock(float dt)
{
	m_clock += m_clockSpeed * dt;
	if (m_clock >= 1) {
		m_clock = 0;
	}
	if (m_clock > m_contractTime) {
		m_restLength = m_contractLength;
		m_edgeThickness = m_contractThickness;
	}
	else {
		m_restLength = m_expandLength;
		m_edgeThickness = m_expandThickness;
	}
}

void Muscle::UpdateInternalForces(float dt)
{
	Vec2f d_p = m_nodeB->m_position - m_nodeA->m_position;
	Vec2f d_v = m_nodeB->m_velocity - m_nodeA->m_velocity;
	float dist = d_p.getLength();
	d_p /= dist; // normalize

	float F_s = (dist - m_restLength) * m_stiffness;
	float F_d = Vec2f::dot(d_p, d_v) * m_damping;

	float F_t = F_s + F_d;

	m_nodeA->m_internalforce += F_t * d_p;
	m_nodeB->m_internalforce += F_t * -d_p;
}

void Muscle::UpdateExternalForces(float dt)
{
	Vec2f normal = Vec2f::getOrthogonal(m_nodeB->m_position - m_nodeA->m_position).normalize();

	float muscleExtentionRatio = Vec2f::distance(m_nodeA->m_position, m_nodeB->m_position) / Config::creature_maxEdgeLength;

	float res_A = Vec2f::dot(normal, m_nodeA->m_internalforce);
	m_nodeA->m_externalForce += -normal * res_A * muscleExtentionRatio * Config::waterFrictionCoef;

	float res_B = Vec2f::dot(normal, m_nodeB->m_internalforce);
	m_nodeB->m_externalForce += -normal * res_B * muscleExtentionRatio * Config::waterFrictionCoef;
}

inline sf::Vector2f toSFVec(Vec2f v) {
	return sf::Vector2f(v.x, v.y);
}

void Muscle::Draw(sf::RenderWindow& window)
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
