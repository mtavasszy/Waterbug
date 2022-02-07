#define _USE_MATH_DEFINES

#include "node.h"
#include <SFML/Graphics.hpp>
#include <math.h>
#include "Vec2.h"


Node::Node(Vec2f position) {
	// physics
	m_position = position;
	m_velocity = Vec2f(0.f);
	m_force = Vec2f(0.f);
	m_mass = 1;

	// draw
	m_circle = sf::CircleShape(m_nodeRadius);
	m_circle.setPointCount(20);
	m_circle.setOutlineColor(sf::Color::Black);
	m_circle.setOutlineThickness(-2.f);
}

void Node::update(float dt)
{
	// euler integration
	m_velocity += (m_force / m_mass) * dt;
	m_position += m_velocity * dt;
	
	// reset force
	m_force = Vec2f(0.f);
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
