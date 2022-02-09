#ifndef CREATURE_H_ 
#define CREATURE_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include "node.h"
#include "muscle.h"

class Creature {

public:
	Creature(bool init, Vec2f startPos);

	// structure
	void generateRandom(Vec2f startPos);
	void addRandomNode(Vec2f startPos);
	bool isCrossingMuscle(Vec2f p0, Vec2f p1);
	bool hasLooseNodeGroups();

	// simulation
	void update(float dt);
	void updateMuscles(float dt);
	void updateNodes(float dt);

	// visualization
	void draw(sf::RenderWindow& window);

	// attributes
	std::vector<std::unique_ptr<Node>> m_nodes;
	std::vector< std::unique_ptr<Muscle>> m_muscles;

	// random
	std::mt19937 m_gen;
	std::uniform_real_distribution<> m_dis_real;
	std::uniform_int_distribution<> m_dis_int;
};

#endif  // CREATURE_H_