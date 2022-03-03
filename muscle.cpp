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

	m_expandLength = Config::creature_maxEdgeLength;
	m_contractLength = Config::creature_minEdgeLength;

	// clock
	m_clockStart = clockStart;
	m_clock = m_clockStart;
	m_restLength = m_expandLength;
	m_contractTime = contractTime;
	m_clockSpeed = Config::creature_clockSpeed;

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
	m_expandLength = Config::creature_maxEdgeLength;
	m_contractLength = Config::creature_minEdgeLength;
	m_restLength = m->m_restLength;
	m_stiffness = m->m_stiffness;
	m_damping = m->m_damping;
	m_clockSpeed = Config::creature_clockSpeed;

	m_muscleExtentionRatio = m->m_muscleExtentionRatio;
	m_normal = m->m_normal;

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

void Muscle::UpdateInternalForces(float dt)
{
	Vec2f d_p = m_nodeB->m_position - m_nodeA->m_position;
	Vec2f d_v = m_nodeB->m_velocity - m_nodeA->m_velocity;
	float dist = d_p.getLength();
	d_p /= dist; // normalize

	float F_s = (dist - m_restLength) * m_stiffness;
	float F_d = Vec2f::dot(d_p, d_v) * m_damping;

	float F_t = F_s + F_d;

	m_nodeA->m_internalForce += F_t * d_p;
	m_nodeB->m_internalForce += F_t * -d_p;
}

void Muscle::UpdateExternalForces(float dt)
{
	m_muscleExtentionRatio = Vec2f::distance(m_nodeA->m_position, m_nodeB->m_position) / Config::creature_maxEdgeLength;

	m_nodeA->ApplyDrag(this);
	m_nodeB->ApplyDrag(this);
	m_nodeA->ApplyPushBack(this);
	m_nodeB->ApplyPushBack(this);
}

void Muscle::Mutate(std::mt19937& gen)
{
	auto mutType_rnd = std::uniform_int_distribution<int>(0, 1);
	auto mutBit_rnd = std::uniform_int_distribution<int>(1, 8); // assume 8 bit genome
	float change = (1.f / powf(2.f, float(mutBit_rnd(gen)))) - 0.5f;

	switch (mutType_rnd(gen))
	{
	case 0:
		//m_clockStart = mut_rnd(gen);
		m_clockStart += change;
		if (m_clockStart < 0.f)
			m_clockStart += 1.f;
		if (m_clockStart > 1.f)
			m_clockStart -= 1.f;
		break;
	case 1:
		//m_contractTime = mut_rnd(gen);
		m_contractTime += change;
		if (m_contractTime < 0.f)
			m_contractTime += 1.f;
		if (m_contractTime > 1.f)
			m_contractTime -= 1.f;
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

	m_edgeVertices[0].position = toSFVec(m_nodeA->m_position + offset - camPos);
	m_edgeVertices[1].position = toSFVec(m_nodeB->m_position + offset - camPos);
	m_edgeVertices[2].position = toSFVec(m_nodeB->m_position - offset - camPos);
	m_edgeVertices[3].position = toSFVec(m_nodeA->m_position - offset - camPos);

	window.draw(m_edgeVertices, 4, sf::Quads);

	if (m_isHullAB) {
		sf::Vertex hullABLine[] =
		{
			sf::Vertex(toSFVec(m_nodeA->m_position + hullOffset - camPos), sf::Color::Black),
			sf::Vertex(toSFVec(m_nodeB->m_position + hullOffset - camPos), sf::Color::Black)
		};
		window.draw(hullABLine, 2, sf::Lines);
	}

	if (m_isHullBA) {
		sf::Vertex hullBALine[] =
		{
			sf::Vertex(toSFVec(m_nodeA->m_position - hullOffset - camPos), sf::Color::Black),
			sf::Vertex(toSFVec(m_nodeB->m_position - hullOffset - camPos), sf::Color::Black)
		};
		window.draw(hullBALine, 2, sf::Lines);
	}

}
