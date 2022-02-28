#include "node.h"
#include <SFML/Graphics.hpp>
#include "Vec2.h"
#include "config.h"
#include "utils.h"
#include "muscle.h"

Node::Node(Vec2f position) {
	// physics
	m_position = position;
	m_velocity = Vec2f(0.f);
	m_internalforce = Vec2f(0.f);
	m_externalForce = Vec2f(0.f);
	m_mass = 1;

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
	m_internalforce = n->m_internalforce;
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
	m_internalforce = Vec2f(0.f);
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
	float res = Vec2f::dot(m->m_normal, m_internalforce);

	if (res > 0 ? m->m_isHullAB : m->m_isHullBA) {
		m_externalForce += -m->m_normal * res * m->m_muscleExtentionRatio * Config::waterFrictionCoef;
	}
}

void Node::ApplyForces(float dt)
{
	// euler integration
	Vec2f force = m_internalforce + m_externalForce;
	m_velocity += (force / m_mass) * dt;
	m_position += m_velocity * dt;
}

void Node::Draw(sf::RenderWindow& window, Vec2f camPos)
{
	m_circle.setPosition(toSFVec(m_position - m_circle.getRadius() - camPos));
	m_circle.setFillColor(m_fillColor);
	window.draw(m_circle);
}
