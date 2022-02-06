#include "creature.h"
#include "node.h"
#include "Vec2.h"


Creature::Creature() {
	m_nodes.push_back(Node(Vec2f(300.f, 300.f)));
	m_nodes.push_back(Node(Vec2f(400.f, 300.f)));
	//m_nodes.push_back(Node(Vec2f(334.f, 320.f)));

	m_muscles.push_back(Muscle(&m_nodes[0], &m_nodes[1]));
	//m_muscles.push_back(Muscle(&m_nodes[1], &m_nodes[2]));
	//m_muscles.push_back(Muscle(&m_nodes[2], &m_nodes[0]));
}

void Creature::update(float dt) {
	// Compute muscle forces
	for (auto it = m_muscles.begin(); it != m_muscles.end(); it++) {
		it->update(dt);
	}
	// Apply forces to nodes
	for (auto it = m_nodes.begin(); it != m_nodes.end(); it++ ) {
		it->update(dt);
	}
}

void Creature::draw(sf::RenderWindow& window) {
	for (auto it = m_muscles.begin(); it != m_muscles.end(); it++) {
		it->draw(window);
	}
	for (auto it = m_nodes.begin(); it != m_nodes.end(); it++) {
		it->draw(window);
	}
}