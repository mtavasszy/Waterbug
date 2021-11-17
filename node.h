#ifndef NODE_H_ 
#define NODE_H_

#include <vector>
#include <SFML/Graphics.hpp>

class Node {

public:
	Node();
	Node(float a0, float a1, float a0_t, float a0_l);
	void simulateStep();
	void updatePosition();
	void addAppendage(Node node);
	void draw(sf::RenderWindow& window, sf::Color color = sf::Color::White);

	Node* parent = nullptr;
	std::vector<Node> appendages;

	// physics information
	float armLength = 40;
	float rotation;
	sf::Vector2f position;
	sf::Vector2f speed;
	sf::Vector2f acceleration;
	sf::Vector2f force;
	float mass;

	// muscle information
	int clock; // runs from 0 to 1

	float currentAngle;
	float angle0, angle1;
	float angle0_t, angle0_l;

	// drawing information
	float nodeRadius = 12;
	float edgeThickness = 4;
};

#endif  // NODE_H_
