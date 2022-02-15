#ifndef CREATURE_H_ 
#define CREATURE_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include "node.h"
#include "muscle.h"
#include "Vec2.h"

class Creature {

public:
	Creature(bool init);
	Creature(const Creature* c);

	// structure
	void GenerateRandom();
	void ReCenter();
	Vec2f GetCenter();
	void AddRandomNode();
	bool IsCrossingMuscle(Vec2f p0, Vec2f p1);
	bool HasLooseNodeGroups();

	// simulation
	void Update(float dt);
	void UpdateMuscles(float dt);
	void UpdateNodes(float dt);

	// visualization
	void Draw(sf::RenderWindow& window, Vec2f camPos);

	// attributes
	std::vector<std::unique_ptr<Node>> m_nodes;
	std::vector< std::unique_ptr<Muscle>> m_muscles;
	float m_fitness;

	// random
	std::mt19937 m_gen;
};

#endif  // CREATURE_H_