#ifndef CREATURE_H_ 
#define CREATURE_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include "node.h"
#include "muscle.h"

class Creature {

public:
	Creature();
	void update(float dt);
	void updateMuscles(float dt);
	void updateNodes(float dt);
	void draw(sf::RenderWindow &window);

	std::vector<Node> m_nodes;
	std::vector<Muscle> m_muscles;
};

#endif  // CREATURE_H_