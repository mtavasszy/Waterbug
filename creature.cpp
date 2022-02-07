#include "creature.h"
#include "node.h"
#include "Vec2.h"


Creature::Creature() {
	m_nodes.push_back(Node(Vec2f(300.f, 300.f)));
	m_nodes.push_back(Node(Vec2f(400.f, 300.f)));
	m_nodes.push_back(Node(Vec2f(350.f, 386.f)));

	m_muscles.push_back(Muscle(&m_nodes[0], &m_nodes[1], 100, 30));
	m_muscles.push_back(Muscle(&m_nodes[1], &m_nodes[2], 100, 50));
	m_muscles.push_back(Muscle(&m_nodes[2], &m_nodes[0], 100, 40));

	//m_nodes.push_back(Node(Vec2f(300.f, 300.f)));
	//m_nodes.push_back(Node(Vec2f(400.f, 300.f)));
	//m_nodes.push_back(Node(Vec2f(300.f, 400.f)));
	//m_nodes.push_back(Node(Vec2f(400.f, 400.f)));

	//m_muscles.push_back(Muscle(&m_nodes[0], &m_nodes[1], 100, 40));
	//m_muscles.push_back(Muscle(&m_nodes[1], &m_nodes[3], 100, 40));
	//m_muscles.push_back(Muscle(&m_nodes[3], &m_nodes[2], 100, 40));
	//m_muscles.push_back(Muscle(&m_nodes[2], &m_nodes[0], 100, 40));
	//m_muscles.push_back(Muscle(&m_nodes[3], &m_nodes[0], 141, 55));
	//m_muscles.push_back(Muscle(&m_nodes[1], &m_nodes[2], 141, 55));
}

void Creature::update(float dt) {
	updateMuscles(dt);
	updateNodes(dt);
}

void Creature::updateMuscles(float dt)
{
	for (auto it = m_muscles.begin(); it != m_muscles.end(); it++) {
		it->update(dt);
	}
}

void Creature::updateNodes(float dt)
{
	for (auto it = m_nodes.begin(); it != m_nodes.end(); it++) {
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