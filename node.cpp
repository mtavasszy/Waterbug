#define _USE_MATH_DEFINES
#include "node.h"
#include <SFML/Graphics.hpp>
#include <math.h>

Node::Node() {}


Node::Node(float a0, float a1, float a0_l) {
	armAngle0 = a0;
	armAngle1 = a1;
	armAngle0_t = a0_l;

	armAnglePos = armAngle1;

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

	if (clock > armMoveTime && clock < armAngle0_t) { // set to armAngle0
		armAnglePos = armAngle0;
		armAngleVel = 0;
		armAngleAcc = 0;
		return;
	}
	else if (clock > armAngle0_t + armMoveTime) { // set to armAngle1
		armAnglePos = armAngle1;
		armAngleVel = 0;
		armAngleAcc = 0;
		return;
	}

	float targetAngleDiff = armAngle1 - armAngle0;
	float targetAcc = (4.f * targetAngleDiff) / armMoveTime;

	if (clock < 0.5f * armMoveTime) // a1 -> a0 accelerate
		armAngleAcc = -targetAcc;
	if (clock > 0.5f * armMoveTime && clock < armMoveTime) // a1 -> a0 decelerate
		armAngleAcc = targetAcc;
	if (clock > armAngle0_t && clock <= armAngle0_t + 0.5f * armMoveTime) // a0 -> a1 accelerate
		armAngleAcc = targetAcc;
	if (clock > armAngle0_t + 0.5f * armMoveTime && clock <= armAngle0_t + armMoveTime) // a0 -> a1 decelerate
		armAngleAcc = -targetAcc;

	armAngleVel += armAngleAcc * deltaClock;
	armAnglePos += armAngleVel * deltaClock;

	if (armAnglePos < armAngle0) {
		armAnglePos = armAngle0;
		armAngleVel = 0;
		armAngleAcc = 0;
	}

	if (armAnglePos > armAngle1) {
		armAnglePos = armAngle1;
		armAngleVel = 0;
		armAngleAcc = 0;
	}
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
		rotation = parent->rotation + armAnglePos;
		position = parent->position + sf::Vector2f(std::cosf(DegToRad(armAnglePos)), std::sinf(DegToRad(armAnglePos))) * armLength;
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