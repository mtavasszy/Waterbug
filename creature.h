#ifndef CREATURE_H_ 
#define CREATURE_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include "node.h"
#include "muscle.h"

class Creature {

public:
	Creature(bool init, Vec2f startPos);
	Creature(const Creature* c);

	// structure
	void GenerateRandom(Vec2f startPos);
	void AddRandomNode(Vec2f startPos);
	bool IsCrossingMuscle(Vec2f p0, Vec2f p1);
	bool HasLooseNodeGroups();

	// simulation
	void Update(float dt);
	void UpdateMuscles(float dt);
	void UpdateNodes(float dt);
	Vec2f GetCenter();

	// visualization
	void Draw(sf::RenderWindow& window);

	// attributes
	std::vector<std::unique_ptr<Node>> m_nodes;
	std::vector< std::unique_ptr<Muscle>> m_muscles;
	float m_fitness;

	// random
	std::mt19937 m_gen;
};

#endif  // CREATURE_H_