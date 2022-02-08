#ifndef CREATURE_H_ 
#define CREATURE_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include "node.h"
#include "muscle.h"

class Creature {

public:
	Creature(bool init);

	// structure
	void generateRandom();

	// simulation
	void update(float dt);
	void updateMuscles(float dt);
	void updateNodes(float dt);

	// visualization
	void draw(sf::RenderWindow &window);

	// attributes
	std::vector<std::unique_ptr<Node>> m_nodes;
	std::vector< std::unique_ptr<Muscle>> m_muscles;

	// random
	std::mt19937 m_gen;
	std::uniform_real_distribution<> m_dis_real;
	std::uniform_int_distribution<> m_dis_int;

	// config
	int m_minStartNodes = 3;
	int m_maxStartNodes = 6;
	float m_edgeConnectChance = 0.5f;
	float m_maxEdgeLength = 100.f; 
	float m_minEdgeLength = 40.f;
};

#endif  // CREATURE_H_