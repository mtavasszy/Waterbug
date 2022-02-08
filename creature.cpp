#include "creature.h"
#include "node.h"
#include "Vec2.h"


Creature::Creature(bool init) {
	//m_nodes.push_back(Node(Vec2f(300.f, 300.f)));
	//m_nodes.push_back(Node(Vec2f(400.f, 300.f)));
	//m_nodes.push_back(Node(Vec2f(350.f, 386.f)));

	//m_muscles.push_back(Muscle(&m_nodes[0], &m_nodes[1], 100, 30));
	//m_muscles.push_back(Muscle(&m_nodes[1], &m_nodes[2], 100, 50));
	//m_muscles.push_back(Muscle(&m_nodes[2], &m_nodes[0], 100, 40));

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

	// random
	std::random_device rd;
	m_gen = std::mt19937(rd()); // Standard mersenne_twister_engine seeded with rd()
	m_dis_int = std::uniform_int_distribution<>(0, 1);
	m_dis_real = std::uniform_real_distribution<>(0.f, 1.f);

	if (init) {
		generateRandom();
	}
}

void Creature::generateRandom()
{
	auto dis_nodes = std::uniform_int_distribution<int>(m_minStartNodes, m_maxStartNodes);
	int nNodes = dis_nodes(m_gen);

	auto dis_pos = std::uniform_real_distribution<float>(300.f, 400.f);
	auto dis_norm = std::uniform_real_distribution<float>(0.f, 1.f);

	// add nodes
	for (int i = 0; i < nNodes; i++) {
		Vec2f pos = Vec2f(dis_pos(m_gen), dis_pos(m_gen));
		m_nodes.push_back(std::make_unique<Node>(Node(pos)));
	}

	// connect muscles
	for (int i = 0; i < nNodes; i++) {
		for (int j = 0; j < i; j++) {
			if (i > 0) {
				auto dis_node = std::uniform_int_distribution<int>(0, i - 1);
				int connectingNode = dis_node(m_gen);
				m_muscles.push_back(std::make_unique<Muscle>(Muscle(m_nodes[i].get(), m_nodes[connectingNode].get(), m_maxEdgeLength, m_minEdgeLength, dis_norm(m_gen), dis_norm(m_gen))));

				// randomly connect to other nodes too
				for (int k = 0; k < i - 1; k++) {
					if (i != connectingNode && dis_norm(m_gen) < m_edgeConnectChance) {
						m_muscles.push_back(std::make_unique<Muscle>(Muscle(m_nodes[i].get(), m_nodes[k].get(), m_maxEdgeLength, m_minEdgeLength, dis_norm(m_gen), dis_norm(m_gen))));
					}
				}
			}
		}
	}
}

void Creature::update(float dt) {
	updateMuscles(dt);
	updateNodes(dt);
}

void Creature::updateMuscles(float dt)
{
	for (int i = 0; i < m_muscles.size(); i++) {
		m_muscles[i]->update(dt);
	}
}

void Creature::updateNodes(float dt)
{
	for (int i = 0; i < m_nodes.size(); i++) {
		m_nodes[i]->update(dt);
	}
}

void Creature::draw(sf::RenderWindow& window) {
	for (int i = 0; i < m_muscles.size(); i++) {
		m_muscles[i]->draw(window);
	}
	for (int i = 0; i < m_nodes.size(); i++) {
		m_nodes[i]->draw(window);
	}
}