#include "node.h"
#include <SFML/Graphics.hpp>
#include "Vec2.h"
#include "config.h"


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

	// drawing information
	m_nodeRadius = n->m_nodeRadius;
	m_circle = n->m_circle;
	m_fillColor = n->m_fillColor;
}

void Node::UpdateExternalForces(float dt)
{
	m_externalForce += -0.5f * m_velocity.getLength() * powf(Config::waterDragCoef, dt) * m_velocity;
}

void Node::ApplyForces(float dt)
{
	// euler integration
	Vec2f force = m_internalforce + m_externalForce;
	m_velocity += (force / m_mass) * dt;
	m_position += m_velocity * dt;

	// reset forces
	m_internalforce = Vec2f(0.f);
	m_externalForce = Vec2f(0.f);
}

inline sf::Vector2f toSFVec(Vec2f v) {
	return sf::Vector2f(v.x, v.y);
}

void Node::Draw(sf::RenderWindow& window, Vec2f camPos)
{
	m_circle.setPosition(toSFVec(m_position - m_circle.getRadius() - camPos));
	m_circle.setFillColor(m_fillColor);
	window.draw(m_circle);
}
