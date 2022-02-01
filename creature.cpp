#include "creature.h"
#include "node.h"
#include "Vec2.h"


Creature::Creature() {
	initialize();
}

void Creature::initialize() {
	m_nodes.push_back(Node(true));
	m_nodes[0].m_position = Vec2f(1280 / 2, 720 / 2);
	m_mainNode = &m_nodes[0];

	m_nodes.push_back(Node(120.f, 140.f, 0.5f));
	m_nodes.push_back(Node(240.f, 280.f, 0.2f));
	m_nodes.push_back(Node(-40.f, 10.f, 0.4f));
	m_nodes[1].setParent(&m_nodes[0]);
	m_nodes[2].setParent(&m_nodes[0]);
	m_nodes[3].setParent(&m_nodes[1]);

	for (auto it = m_nodes.begin(); it != m_nodes.end(); it++) {
		it->updatePosition();
	}
}

void Creature::update(float dt) {
	for (auto it = m_nodes.begin(); it != m_nodes.end(); it++ ) {
		it->update(dt);
	}
}

void Creature::draw(sf::RenderWindow& window) {
	for (auto it = m_nodes.begin(); it != m_nodes.end(); it++) {
		it->drawEdge(window);
	}
	for (auto it = m_nodes.begin(); it != m_nodes.end(); it++) {
		it->drawNode(window);
	}
}