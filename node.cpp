#define _USE_MATH_DEFINES
#include "node.h"
#include <SFML/Graphics.hpp>
#include <math.h>

Node::Node() {}


Node::Node(float a0, float a1, float a0_l) {
	angle0 = a0;
	angle1 = a1;
	angle0_l = a0_l;

	currentAngle = angle0;

	clockStart = 0; // random?
	clock = clockStart;

	rotation = 0;
	position = sf::Vector2f(0.f, 0.f);
	speed = sf::Vector2f(0.f, 0.f);
	acceleration = sf::Vector2f(0.f, 0.f);
	force = sf::Vector2f(0.f, 0.f);
	mass = 1;

	appendages = std::vector<Node>();
}

void Node::updateCurrentAngle() {
	float currentAngleCoeff = 0; // 0 represents a0, 1 is a1

	float m = 0.f; // a0

	if (clock > angle0_l + moveTime) // a1
		m = 1.f;
	else if (clock < moveTime) // move from a1 to a0
		m = 1.f - (clock / moveTime);
	else if (clock > angle0_l && clock <= angle0_l + moveTime) // move from a0 to a1
		m = (clock - angle0_l) / moveTime;

	if (m < 0.5f)
		currentAngleCoeff = 2.f * m * m; // acceleration
	else
		currentAngleCoeff = 1.f - 2.f * ((1.f - m) * (1.f - m)); // deceleration

	currentAngle = angle1 * currentAngleCoeff + angle0 * (1 - currentAngleCoeff);
}

void Node::simulateStep() {
	if (parent != nullptr) { // exclude body node
		updateCurrentAngle();

		clock += deltaClock;
		if (clock > 1.f) {
			clock = 0;
		}
	}

	for (int i = 0; i < appendages.size();i++) {
		Node* n = &appendages[i];
		n->simulateStep();
	}
}

inline float DegToRad(float Deg)
{
	return Deg / 180.f * M_PI;
}


void Node::updatePosition()
{
	if (parent != nullptr) { // exclude body node
		rotation = parent->rotation + currentAngle;
		position = parent->position + sf::Vector2f(std::cosf(DegToRad(currentAngle)), std::sinf(DegToRad(currentAngle))) * armLength;
	}

	for (int i = 0; i < appendages.size(); i++) {
		Node* n = &appendages[i];
		n->updatePosition();
	}
}


void Node::addAppendage(Node node)
{
	node.parent = this;
	node.updatePosition();
	appendages.push_back(node);
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