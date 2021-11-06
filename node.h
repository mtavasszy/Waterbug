#ifndef NODE_H_ 
#define NODE_H_

#include <vector>
#include <SFML/Graphics.hpp>
//#include "edge.h"

class Node {

public:
	Node();
	Node(sf::Vector2f pos);
	//void simulateStep();
	void draw(sf::RenderWindow& window, sf::Color color = sf::Color::White);

	std::vector<Node> appendages;

	sf::Vector2f position;
	sf::Vector2f speed;
	sf::Vector2f force;
	float mass;

	// movement information
	int clock;

	float a0, a1;
	float t0, t1;

	// drawing information
	float nodeRadius = 12;
	float edgeThickness = 4;
};

#endif  // NODE_H_
