#ifndef CREATURE_H_ 
#define CREATURE_H_

#include <SFML/Graphics.hpp>
#include "node.h"

class Creature {

public:
	Creature();
	void initialize();
	void simulateStep();
	void simulate();
	void draw(sf::RenderWindow &window);

	Node bodyNode;
};

#endif  // CREATURE_H_