#ifndef MUSCLE_H_ 
#define MUSCLE_H_

#include <SFML/Graphics.hpp>
#include <random>
#include "node.h"

class Creature;

class Muscle {

public:
	Muscle(Creature * parent, int A_i, int B_i, float clockStart, float contractTime);
	Muscle(const Muscle* m);

	void ResetNodePointers(Creature* parent);
	void HandleDelete();
	void UpdateClock(float dt);
	void SetNormal();
	bool ContainsNode(int n);
	int GetOther(int nodeId);
	void UpdateInternalForces(float dt);
	void UpdateExternalForces(float dt);
	void Mutate(std::mt19937& gen);
	void Draw(sf::RenderWindow& window, Vec2f camPos);

	// nodes
	int m_Ai, m_Bi;
	Node* m_nodeA;
	Node* m_nodeB;

	// spring model
	float m_expandLength = 100.f;
	float m_contractLength = 40.f;
	float m_restLength;
	float m_stiffness = 100.f;
	float m_damping = 10.f;

	// simulation
	Vec2f m_normal;
	float m_muscleExtentionRatio;
	bool m_isHullAB;
	bool m_isHullBA;

	// movement
	float m_clock; // runs from 0 to 1
	float m_clockStart = 0.f;
	float m_clockSpeed = 0.5f;
	float m_contractTime = 0.5f; //TODO cleanup default values that do nothing

	// drawing information
	float m_expandThickness = 4.f;
	float m_contractThickness = 8.f;
	float m_edgeThickness;
	sf::Vertex m_edgeVertices[4];
};

#endif  // MUSCLE_H_
