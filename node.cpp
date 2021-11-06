#include "node.h"
#include <SFML/Graphics.hpp>

Node::Node() {}


Node::Node(sf::Vector2f pos) {
	position = pos;
	speed = sf::Vector2f(0.f, 0.f);
	force = sf::Vector2f(0.f,0.f);
	mass = 1;

	appendages = std::vector<Node>();
}


void Node::draw(sf::RenderWindow& window, sf::Color color) {

	// draw edges
	for (Node node : appendages) {
		const sf::Vector2f direction = position - node.position;
		const sf::Vector2f unitDirection = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y);
		const sf::Vector2f unitPerpendicular(-unitDirection.y, unitDirection.x);

		sf::Vector2f offset = (edgeThickness / 2.f) * unitPerpendicular;

		sf::Vertex vertices[4];
		vertices[0].position = position + offset;
		vertices[1].position = node.position + offset;
		vertices[2].position = node.position - offset;
		vertices[3].position = position - offset;

		for (int i = 0; i < 4; ++i)
			vertices[i].color = sf::Color::White;

		window.draw(vertices, 4, sf::Quads);
	}

	// draw appendage nodes
	for (Node node : appendages) {
		node.draw(window);
	}

	// draw the node itself
	sf::CircleShape circle = sf::CircleShape(nodeRadius);
	circle.setPointCount(20);
	circle.setPosition(position - sf::Vector2f(circle.getRadius(), circle.getRadius()));
	circle.setFillColor(color);
	circle.setOutlineColor(sf::Color::Black);
	circle.setOutlineThickness(-2.f);
	window.draw(circle);
}