#ifndef CREATURE_H_ 
#define CREATURE_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include "node.h"

class Creature {

public:
	Creature();
	void initialize();
	void update(float dt);
	void draw(sf::RenderWindow &window);

	Node* m_mainNode;
	std::vector<Node> m_nodes;
};

#endif  // CREATURE_H_