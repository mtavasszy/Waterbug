#pragma once
#ifndef MUSCLE_H_ 
#define MUSCLE_H_

#include <SFML/Graphics.hpp>
#include <random>
#include "node.h"


class Muscle {

public:
	Muscle(Node* A, Node* B, float expandLength, float contractLength, float clockStart, float contractTime);
	void update(float dt);
	void updateClock(float dt);
	void updateForces(float dt);
	void draw(sf::RenderWindow& window);

	Node* m_nodeA;
	Node* m_nodeB;

	// spring model info
	float m_expandLength = 100.f;
	float m_contractLength = 40.f;
	float m_restLength;
	float m_stiffness = 100.f;
	float m_damping = 10.f;

	// movement
	float m_clock; // runs from 0 to 1
	float m_clockStart = 0.f;
	float m_clockSpeed = 0.5f;

	float m_contractTime = 0.5f;

	// drawing information
	float m_expandThickness = 4.f;
	float m_contractThickness = 8.f;
	float m_edgeThickness;
	sf::Vertex m_edgeVertices[4];
};

#endif  // MUSCLE_H_