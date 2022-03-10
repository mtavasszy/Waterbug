#include "muscle.h"
#include "node.h"
#include "Vec2.h"
#include "config.h"
#include "creature.h"
#include "utils.h"

Muscle::Muscle(Creature* parent, int A_i, int B_i, float clockStart, float contractTime)
{
	// connected nodes
	m_Ai = A_i;
	m_Bi = B_i;
	ResetNodePointers(parent);

	m_nodeA->m_connectedNodes.push_back(m_Bi);
	m_nodeB->m_connectedNodes.push_back(m_Ai);

	m_expandLength = MAX_MUSCLE_LENGTH;
	m_contractLength = MIN_MUSCLE_LENGTH;

	// clock
	m_clockStart = clockStart;
	m_clock = m_clockStart;
	m_restLength = m_expandLength;
	m_contractTime = contractTime;
	m_clockSpeed = CLOCK_SPEED;

	// draw
	for (int i = 0; i < 4; ++i)
		m_edgeVertices[i].color = sf::Color::White;
}

Muscle::Muscle(const Muscle* m)
{
	// connected nodes
	m_Ai = m->m_Ai;
	m_Bi = m->m_Bi;

	// spring model info
	m_expandLength = MAX_MUSCLE_LENGTH;
	m_contractLength = MIN_MUSCLE_LENGTH;
	m_restLength = m->m_restLength;
	m_stiffness = m->m_stiffness;
	m_damping = m->m_damping;
	m_clockSpeed = CLOCK_SPEED;

	m_normal = m->m_normal;

	// movement
	m_clock = m->m_clock; // runs from 0 to 1
	m_clockStart = m->m_clockStart;
	m_clockSpeed = m->m_clockSpeed;
	m_contractTime = m->m_contractTime;

	m_isHullAB = m->m_isHullAB;
	m_isHullBA = m->m_isHullBA;

	// drawing information
	m_expandThickness = m->m_expandThickness;
	m_contractThickness = m->m_contractThickness;
	m_edgeThickness = m->m_edgeThickness;
	for (int i = 0; i < 4; i++) {
		m_edgeVertices[i] = m->m_edgeVertices[i];
	}
}

void Muscle::ResetNodePointers(Creature* parent)
{
	m_nodeA = parent->m_nodes[m_Ai].get();
	m_nodeB = parent->m_nodes[m_Bi].get();

}

void Muscle::HandleDelete()
{
	for (int i = 0; i < m_nodeA->m_connectedNodes.size(); i++) {
		if (m_nodeA->m_connectedNodes[i] == m_Bi) {
			m_nodeA->m_connectedNodes.erase(m_nodeA->m_connectedNodes.begin() + i);
			break;
		}
	}
	for (int i = 0; i < m_nodeB->m_connectedNodes.size(); i++) {
		if (m_nodeB->m_connectedNodes[i] == m_Ai) {
			m_nodeB->m_connectedNodes.erase(m_nodeB->m_connectedNodes.begin() + i);
			break;
		}
	}
}

void Muscle::UpdateClock()
{
	m_clock += m_clockSpeed * RUN_DT;
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

void Muscle::SetNormal()
{
	const Vec2f AtoB = m_nodeB->m_position - m_nodeA->m_position;
	m_normal = Vec2f::getOrthogonal(AtoB).normalize();
}

bool Muscle::ContainsNode(int n)
{
	return m_Ai == n || m_Bi == n;
}

int Muscle::GetOther(int nodeId)
{
	if (nodeId == m_Ai)
		return m_Bi;
	else if (nodeId == m_Bi)
		return m_Ai;

	return -1;
}

bool Muscle::IsWithinBoundingBox(Vec2f p)
{
	Vec2f bbMin, bbMax;
	if (m_nodeA->m_position.x >= m_nodeB->m_position.x) {
		bbMin.x = m_nodeB->m_position.x - NODE_RADIUS;
		bbMax.x = m_nodeA->m_position.x - NODE_RADIUS;
	}
	else {
		bbMin.x = m_nodeA->m_position.x - NODE_RADIUS;
		bbMax.x = m_nodeB->m_position.x - NODE_RADIUS;
	}

	if (m_nodeA->m_position.y >= m_nodeB->m_position.y) {
		bbMin.y = m_nodeB->m_position.y + NODE_RADIUS;
		bbMax.y = m_nodeA->m_position.y + NODE_RADIUS;
	}
	else {
		bbMin.y = m_nodeA->m_position.y + NODE_RADIUS;
		bbMax.y = m_nodeB->m_position.y + NODE_RADIUS;
	}

	return p.x >= bbMin.x && p.x <= bbMax.x && p.y >= bbMin.y && p.y <= bbMax.y;
}

void Muscle::UpdateInternalForces()
{
	Vec2f d_p = m_nodeB->m_position - m_nodeA->m_position;
	if (d_p == Vec2f(0.f)) {
		d_p = Vec2f(0.1f, 0);
		m_nodeB->m_position += d_p;
	}
	const float dist = d_p.getLength();
	d_p /= dist; // normalize

	const Vec2f d_v = m_nodeB->m_velocity - m_nodeA->m_velocity;

	const float F_s = (dist - m_restLength) * STIFFNESS_COEF;
	const float F_d = Vec2f::dot(d_p, d_v) * DAMPING_COEF;

	const float F_t = F_s + F_d;

	m_nodeA->m_internalForce += F_t * d_p;
	m_nodeB->m_internalForce += F_t * -d_p;
}

void Muscle::Mutate(std::mt19937& gen)
{
	auto mutType_rnd = std::uniform_int_distribution<int>(0, 1);
	auto mutBit_rnd = std::uniform_int_distribution<int>(0, 7); // assume 8 bit gene

	switch (mutType_rnd(gen))
	{
	case 0:
		m_clockStart = Utils::mutateFloat(m_clockStart, mutBit_rnd(gen));
		break;
	case 1:
		m_contractTime = Utils::mutateFloat(m_contractTime, mutBit_rnd(gen));
		break;
	default:
		break;
	}
}

void Muscle::Draw(sf::RenderWindow& window, Vec2f camPos)
{
	Vec2f offset = (m_edgeThickness / 2.f) * m_normal;
	Vec2f hullOffset = ((m_edgeThickness / 2.f) + 1.f) * m_normal;

	m_edgeVertices[0].color = sf::Color::White;
	m_edgeVertices[1].color = sf::Color::White;
	m_edgeVertices[2].color = sf::Color::White;
	m_edgeVertices[3].color = sf::Color::White;

	m_edgeVertices[0].position = Utils::toSFVec(m_nodeA->m_position + offset - camPos);
	m_edgeVertices[1].position = Utils::toSFVec(m_nodeB->m_position + offset - camPos);
	m_edgeVertices[2].position = Utils::toSFVec(m_nodeB->m_position - offset - camPos);
	m_edgeVertices[3].position = Utils::toSFVec(m_nodeA->m_position - offset - camPos);

	window.draw(m_edgeVertices, 4, sf::Quads);

	if (m_isHullAB) {
		sf::Vertex hullABLine[] =
		{
			sf::Vertex(Utils::toSFVec(m_nodeA->m_position + hullOffset - camPos), sf::Color::Black),
			sf::Vertex(Utils::toSFVec(m_nodeB->m_position + hullOffset - camPos), sf::Color::Black)
		};
		window.draw(hullABLine, 2, sf::Lines);
	}

	if (m_isHullBA) {
		sf::Vertex hullBALine[] =
		{
			sf::Vertex(Utils::toSFVec(m_nodeA->m_position - hullOffset - camPos), sf::Color::Black),
			sf::Vertex(Utils::toSFVec(m_nodeB->m_position - hullOffset - camPos), sf::Color::Black)
		};
		window.draw(hullBALine, 2, sf::Lines);
	}

}
