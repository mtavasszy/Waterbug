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
	void TrySettleStructure();
	void ReCenter();
	Vec2f GetCenter();
	void CheckIfValidCreature();
	bool HasLooseNodeGroups();
	bool HasCrossingMuscles();
	bool IsCrossingMuscle(int A, int B);
	void AddRandomNode();
	void AddRandomMuscle();
	void RemoveRandomNode();
	void RemoveRandomMuscle();

	// simulation
	void Update(float dt);
	int GetMuscle(int A, int B);
	void SetHull();
	void UpdateMuscles(float dt);
	void UpdateNodes(float dt);
	float ComputeFitness();
	bool IsExploded();

	// reproduction
	Creature createOffspring();
	void Mutate();

	// visualization
	void Draw(sf::RenderWindow& window, Vec2f camPos);

	// attributes
	std::vector<std::unique_ptr<Node>> m_nodes;
	std::vector< std::unique_ptr<Muscle>> m_muscles;

	bool m_isValid = false;
	bool m_isSettled = false;
	float m_fitness;

	// random
	std::mt19937 m_gen;
};

#endif  // CREATURE_H_