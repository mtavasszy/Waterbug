#include "node.h"
#include <SFML/Graphics.hpp>
#include "Vec2.h"
#include "config.h"
#include "utils.h"
#include "muscle.h"
#include "creature.h"

Node::Node(Vec2f position) {
	// physics
	m_position = position;
	m_velocity = Vec2f(0.f);
	m_internalForce = Vec2f(0.f);
	m_externalForce = Vec2f(0.f);
	m_mass = 1.f;

	// draw
	m_circle = sf::CircleShape(m_nodeRadius);
	m_circle.setPointCount(20);
	m_circle.setOutlineColor(sf::Color::Black);
	m_circle.setOutlineThickness(-2.f);
}

Node::Node(const Node* n)
{
	// physics information
	m_position = n->m_position;
	m_velocity = n->m_velocity;
	m_internalForce = n->m_internalForce;
	m_externalForce = n->m_externalForce;
	m_mass = n->m_mass;

	m_connectedNodes = n->m_connectedNodes;

	// drawing information
	m_nodeRadius = n->m_nodeRadius;
	m_circle = n->m_circle;
	m_fillColor = n->m_fillColor;
}

void Node::ResetForces()
{
	m_internalForce = Vec2f(0.f);
	m_externalForce = Vec2f(0.f);
}

void Node::ApplyDrag(Muscle* m)
{
	// check orientation / hull

	float angleCoeff = Vec2f::dot(m->m_normal, m_velocity);

	if (angleCoeff > 0 ? m->m_isHullAB : m->m_isHullBA) {
		if (angleCoeff < 0)
			angleCoeff = -angleCoeff;
		m_externalForce += -0.5f * angleCoeff * powf(Config::waterDragCoef, Config::dt) * m_velocity * m->m_muscleExtentionRatio;
	}
}

void Node::ApplyPushBack(Muscle* m)
{
	// check orientation / hull
	float res = Vec2f::dot(m->m_normal, m_internalForce);

	if (res > 0 ? m->m_isHullAB : m->m_isHullBA) {
		m_externalForce += -m->m_normal * res * m->m_muscleExtentionRatio * Config::waterFrictionCoef;
	}
}

void Node::CorrectCollisions(Creature* parent, int nodeId)
{
	for (int i = 0; i < parent->m_muscles.size(); i++) {
		// check if muscle is colliding
		Muscle* m = parent->m_muscles[i].get();
		if (!m->ContainsNode(nodeId)) {
			const float t = Utils::closestLinePoint(m->m_nodeA->m_position, m->m_nodeB->m_position, m_position);
			const Vec2f closestPoint = Vec2f::interpolate(m->m_nodeA->m_position, m->m_nodeB->m_position, t);
			const Vec2f pToNode = m_position - closestPoint;
			const float sqrDist = pToNode.getSquaredLength();
			if (sqrDist < (m_nodeRadius * m_nodeRadius)) {
				// restore position
				const float dist = std::sqrt(sqrDist);
				const Vec2f pToNodeNorm = pToNode / dist;

				m_position = closestPoint + pToNodeNorm * m_nodeRadius * 1.001f;
				// TODO restore positions relative to speeds

				// preservation of kinetic energy
				const Vec2f v_p = Vec2f::interpolate(m->m_nodeA->m_velocity, m->m_nodeB->m_velocity, t);

				const float v_p_coll = Vec2f::dot(v_p, pToNodeNorm);
				const float v_n_coll = Vec2f::dot(m_velocity, pToNodeNorm);
				if (v_p_coll > 0 || v_n_coll < 0) {// colliding objects are not moving away from each other
					float v_p_coll_new = 0.333f * v_p_coll + 0.666f * v_n_coll;
					float v_n_coll_new = 0.333f * v_n_coll + 0.666f * v_p_coll;
					
					m->m_nodeA->m_velocity -= (1.f - t) * v_p_coll * pToNodeNorm;
					m->m_nodeA->m_velocity += (1.f - t) * v_p_coll_new * pToNodeNorm;
					m->m_nodeB->m_velocity -= t * v_p_coll * pToNodeNorm;
					m->m_nodeB->m_velocity += t * v_p_coll_new * pToNodeNorm;

					m_velocity -= v_n_coll * pToNodeNorm;
					m_velocity += v_n_coll_new * pToNodeNorm;
				}
			}
		}
	}
}

void Node::ApplyForces(float dt)
{
	// euler integration
	Vec2f force = m_internalForce + m_externalForce;
	m_velocity += (force / m_mass) * dt;
	m_position += m_velocity * dt;
}

void Node::Draw(sf::RenderWindow& window, Vec2f camPos)
{
	m_circle.setPosition(Utils::toSFVec(m_position - m_circle.getRadius() - camPos));
	m_circle.setFillColor(m_fillColor);
	window.draw(m_circle);
}
