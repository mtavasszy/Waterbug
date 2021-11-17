#ifndef NODE_H_ 
#define NODE_H_

#include <vector>
#include <SFML/Graphics.hpp>

class Node {

public:
	Node();
	Node(float a0, float a1, float a0_l);
	void updateCurrentAngle();
	void simulateStep();
	void updatePosition();
	void addAppendage(Node node);
	void draw(sf::RenderWindow& window, sf::Color color = sf::Color::White);

	Node* parent = nullptr;
	std::vector<Node> appendages;

	// physics information
	float armLength = 40.f;
	float rotation;
	sf::Vector2f position;
	sf::Vector2f speed;
	sf::Vector2f acceleration;
	sf::Vector2f force;
	float mass;

	// muscle information
	float clock; // runs from 0 to 1
	float clockStart;
	float deltaClock = 0.001f;
	float moveTime = 0.2f;

	float currentAngle;
	float angle0, angle1;
	float angle0_l;

	// drawing information
	float nodeRadius = 12.f;
	float edgeThickness = 4.f;
};

#endif  // NODE_H_
