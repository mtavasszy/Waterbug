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
	m_dragForce = Vec2f(0.f);
	//m_reactionForce = Vec2f(0.f);
	m_mass = 1.f;
	m_nodeRadius = NODE_RADIUS;
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
	m_dragForce = n->m_dragForce;
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
	m_dragForce = Vec2f(0.f);
}

void Node::ApplyDrag(Creature* parent, int nodeId)
{
	const float velocityMag = m_velocity.getLength();
	const Vec2f velocityNorm = m_velocity / velocityMag;
	const Vec2f spanVec = Vec2f::getOrthogonal(velocityNorm);
	float spanMax = m_nodeRadius * 0.47f;
	float spanMin = -m_nodeRadius * 0.47f;

	if (m_connectedNodes.empty())
		return;

	bool isOnHull = false;

	for (int i = 0; i < m_connectedNodes.size(); i++) {

		int connectedMuscle = -1;
		for (int j = 0; j < parent->m_muscles.size(); j++) {
			if (parent->m_muscles[j]->ContainsNode(nodeId) && parent->m_muscles[j]->ContainsNode(m_connectedNodes[i])) {
				connectedMuscle = j;
				break;
			}
		}

		if (Vec2f::dot(velocityNorm, parent->m_muscles[connectedMuscle]->m_normal) > 0 ? parent->m_muscles[connectedMuscle]->m_isHullAB : parent->m_muscles[connectedMuscle]->m_isHullBA) {
			const Vec2f vec2node = parent->m_nodes[m_connectedNodes[i]]->m_position - m_position;
			const float extendAlongSpan = Vec2f::dot(spanVec, vec2node);
			if (extendAlongSpan > 0)
				spanMax = std::max(spanMax, extendAlongSpan);
			if (extendAlongSpan < 0)
				spanMin = std::min(spanMin, extendAlongSpan);

			isOnHull = true;
		}
	}

	if (!isOnHull)
		return;

	float spanLength = spanMax - spanMin;
	float surfaceCoeff = std::min(1.f, spanLength / (MAX_MUSCLE_LENGTH * 2.f));

	Vec2f drag = -0.5f * surfaceCoeff * velocityMag * m_velocity * WATER_DRAG_COEF_DT;
	m_dragForce += drag;
}

void Node::CorrectCollisions(Creature* parent, int nodeId)
{
	for (int i = 0; i < parent->m_muscles.size(); i++) {
		// check if muscle is colliding
		Muscle* m = parent->m_muscles[i].get();
		if (!m->ContainsNode(nodeId)) {
			if (m->IsWithinBoundingBox(m_position)) {

				const float t = Utils::closestLinePoint(m->m_nodeA->m_position, m->m_nodeB->m_position, m_position);
				const Vec2f closestPoint = Vec2f::interpolate(m->m_nodeA->m_position, m->m_nodeB->m_position, t);
				const Vec2f pToNode = m_position - closestPoint;
				const float sqrDist = pToNode.getSquaredLength();
				if (sqrDist < (m_nodeRadius * m_nodeRadius)) {
					// restore position
					const float dist = std::sqrt(sqrDist);
					const Vec2f pToNodeNorm = pToNode / dist;

					// TODO restore positions relative to speeds
					m_position = closestPoint + pToNodeNorm * m_nodeRadius * 1.001f;

					// elastic collision
					const Vec2f v_p = Vec2f::interpolate(m->m_nodeA->m_velocity, m->m_nodeB->m_velocity, t);

					const float v_p_coll = Vec2f::dot(v_p, pToNodeNorm);
					const float v_n_coll = Vec2f::dot(m_velocity, pToNodeNorm);
					if (v_p_coll > 0 || v_n_coll < 0) {// colliding objects are not moving away from each other
						Vec2f v_p_new = v_p - 0.666f * (Vec2f::dot(v_p - m_velocity, -pToNode) / sqrDist) * -pToNode;
						m_velocity = m_velocity - 1.333f * (Vec2f::dot(m_velocity - v_p, pToNode) / sqrDist) * pToNode;

						m->m_nodeA->m_velocity = (1.f - t) * v_p_new;
						m->m_nodeB->m_velocity = t * v_p_new;
					}
				}

			}
		}
	}
}

void Node::ApplyForces()
{
	// euler integration
	Vec2f force = m_internalForce;// +m_dragForce;// + m_reactionForce;
	m_velocity += (force / m_mass) * RUN_DT;

	const float velocitySqr = m_velocity.getSquaredLength();
	if (velocitySqr > MAX_VELOCITY * MAX_VELOCITY) {
		m_velocity = (m_velocity / std::sqrt(velocitySqr)) * MAX_VELOCITY;
	}

	m_position += m_velocity * RUN_DT;
}

void Node::Draw(sf::RenderWindow& window, Vec2f camPos)
{
	m_circle.setPosition(Utils::toSFVec(m_position - m_circle.getRadius() - camPos));
	m_circle.setFillColor(m_fillColor);
	window.draw(m_circle);

	//sf::Vertex FLine[] =
	//{
	//	sf::Vertex(Utils::toSFVec(m_position - camPos), sf::Color::Yellow),
	//	sf::Vertex(Utils::toSFVec(m_position + m_dragForce * 0.04f -camPos), sf::Color::Yellow)
	//};
	//window.draw(FLine, 2, sf::Lines);
}
