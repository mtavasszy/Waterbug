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

void Node::updateExternalForces(float dt)
{
	m_externalForce += -0.5f * m_velocity.getLength() * powf(Config::waterDragCoef, dt) * m_velocity;
}

void Node::applyForces(float dt)
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

void Node::draw(sf::RenderWindow& window)
{
	m_circle.setPosition(toSFVec(m_position - m_circle.getRadius()));
	m_circle.setFillColor(m_fillColor);
	window.draw(m_circle);
}
