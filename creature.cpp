#include "creature.h"
#include "config.h"
#include "node.h"
#include "Vec2.h"
#include <queue>
#include <set>

Creature::Creature(bool init) {

	// random
	std::random_device rd;
	m_gen = std::mt19937(rd()); // Standard mersenne_twister_engine seeded with rd()
	m_fitness = 0.f;

	// init creature
	if (init) {
		do {
			m_nodes.clear();
			m_muscles.clear();
			GenerateRandom();
		} while (HasLooseNodeGroups());
	}

	// stabilize creature
	ReCenter();
}

Creature::Creature(const Creature* c)
{
	m_gen = c->m_gen;

	for (int i = 0; i < c->m_nodes.size(); i++) {
		m_nodes.push_back(std::make_unique<Node>(Node(c->m_nodes[i].get())));
	}
	for (int i = 0; i < c->m_muscles.size(); i++) {
		m_muscles.push_back(std::make_unique<Muscle>(Muscle(c->m_muscles[i].get())));
		m_muscles[i]->m_nodeA = m_nodes[m_muscles[i]->m_nodeAIndex].get();
		m_muscles[i]->m_nodeB = m_nodes[m_muscles[i]->m_nodeBIndex].get();
	}
}

void Creature::GenerateRandom()
{
	auto dis_nodes = std::uniform_int_distribution<int>(Config::creature_minNodes, Config::creature_maxNodes);
	int nNodes = dis_nodes(m_gen);

	// add nodes
	for (int i = 0; i < nNodes; i++) {
		AddRandomNode();
	}
}

void Creature::ReCenter()
{
	Vec2f center = GetCenter();
	for (int i = 0; i < m_nodes.size(); i++) {
		m_nodes[i]->m_position -= center;
	}
}

Vec2f Creature::GetCenter()
{
	Vec2f center = Vec2f(0.f);

	for (int i = 0; i < m_nodes.size(); i++) {
		center += m_nodes[i]->m_position;
	}

	return center / float(m_nodes.size());
}

bool Creature::IsCrossingMuscle(Vec2f p0, Vec2f p1)
{
	const Vec2f p = p0;
	const Vec2f r = p1 - p0;

	for (int i = 0; i < m_muscles.size(); i++) {
		const Vec2f m0 = m_muscles[i]->m_nodeA->m_position;
		const Vec2f m1 = m_muscles[i]->m_nodeB->m_position;

		if (p0 == m0 || p0 == m1 || p1 == m0 || p1 == m1)
			continue; // lines start at same node

		const Vec2f q = m0;
		const Vec2f s = m1 - m0;

		float t, u;
		t = (-r.y * (p.x - q.x) + r.x * (p.y - q.y)) / (-s.x * r.y + r.x * s.y);
		u = (s.x * (p.y - q.y) - s.y * (p.x - q.x)) / (-s.x * r.y + r.x * s.y);

		if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
		{
			return true;
		}
	}

	return false;
}

bool Creature::HasLooseNodeGroups()
{
	std::queue<Node*> nodesToVisit;
	std::set<Node*> visitedNodes;
	nodesToVisit.push(m_nodes[0].get());

	while (!nodesToVisit.empty()) {
		Node* n = nodesToVisit.front();
		visitedNodes.insert(n);
		nodesToVisit.pop();

		for (int j = 0; j < m_muscles.size(); j++) {
			if (m_muscles[j]->m_nodeA->m_position == n->m_position && visitedNodes.count(m_muscles[j]->m_nodeB) == 0) {
				nodesToVisit.push(m_muscles[j]->m_nodeB);
			}
			if (m_muscles[j]->m_nodeB->m_position == n->m_position && visitedNodes.count(m_muscles[j]->m_nodeA) == 0) {
				nodesToVisit.push(m_muscles[j]->m_nodeA);
			}
		}
	}
	return visitedNodes.size() != m_nodes.size();
}

void Creature::AddRandomNode()
{
	auto dis_pos = std::uniform_real_distribution<float>(-50.f, 50.f);
	auto dis_norm = std::uniform_real_distribution<float>(0.f, 1.f);

	Vec2f pos = Vec2f(dis_pos(m_gen), dis_pos(m_gen));
	m_nodes.push_back(std::make_unique<Node>(Node(pos)));

	// connect muscles
	if (m_nodes.size() > 1) {
		for (int i = 0; i < m_nodes.size() - 1; i++) {
			if (dis_norm(m_gen) < Config::creature_edgeConnectChance/* && !isCrossingMuscle(pos, m_nodes[i]->m_position)*/) {
				int n0_i = int(m_nodes.size()) - 1;
				int n1_i = i;
				Node* n0 = m_nodes.back().get();
				Node* n1 = m_nodes[i].get();

				m_muscles.push_back(std::make_unique<Muscle>(Muscle(n0_i, n1_i, n0, n1, Config::creature_maxEdgeLength, Config::creature_minEdgeLength, dis_norm(m_gen), dis_norm(m_gen))));
			}
		}
	}
}

void Creature::AddRandomMuscle()
{
	int maxNoMuscles = (int(m_nodes.size()) * (int(m_nodes.size()) - 1)) / 2;
	if (m_muscles.size() < maxNoMuscles) {

		auto node_rnd = std::uniform_int_distribution<int>(0, int(m_nodes.size()) - 1);
		auto dis_norm = std::uniform_real_distribution<float>(0.f, 1.f);

		int n0_i, n1_i;
		bool muscleExists;

		do {
			n0_i = node_rnd(m_gen);
			do {
				n1_i = node_rnd(m_gen);
			} while (n0_i == n1_i);

			muscleExists = false;
			for (int i = 0; i < m_muscles.size(); i++) {
				if ((m_muscles[i]->m_nodeAIndex == n0_i && m_muscles[i]->m_nodeBIndex == n1_i) || (m_muscles[i]->m_nodeAIndex == n1_i && m_muscles[i]->m_nodeBIndex == n0_i)) {
					muscleExists = true;
					break;
				}
			}
		} while (muscleExists);

		Node* n0 = m_nodes[n0_i].get();
		Node* n1 = m_nodes[n1_i].get();
		m_muscles.push_back(std::make_unique<Muscle>(Muscle(n0_i, n1_i, n0, n1, Config::creature_maxEdgeLength, Config::creature_minEdgeLength, dis_norm(m_gen), dis_norm(m_gen))));
	}
}

void Creature::RemoveRandomNode()
{
	if (m_nodes.size() > Config::creature_minNodes) {
		auto node_rnd = std::uniform_int_distribution<int>(0, int(m_nodes.size()) - 1);
		int n_i = node_rnd(m_gen);

		// remove connecting muscles
		for (int i = 0; i < m_muscles.size(); i++) {
			if (m_muscles[i]->m_nodeAIndex == n_i || m_muscles[i]->m_nodeBIndex == n_i) {
				m_muscles.erase(m_muscles.begin() + i);
				i--;
			}
		}

		// remove node
		m_nodes.erase(m_nodes.begin() + n_i);
	}
}

void Creature::RemoveRandomMuscle()
{
	auto muscle_rnd = std::uniform_int_distribution<int>(0, int(m_muscles.size()) - 1);
	int m_i = muscle_rnd(m_gen);
	m_muscles.erase(m_muscles.begin() + m_i);
}


void Creature::Update(float dt) {
	UpdateMuscles(dt);
	UpdateNodes(dt);
}

void Creature::UpdateMuscles(float dt)
{
	for (int i = 0; i < m_muscles.size(); i++) {
		m_muscles[i]->UpdateClock(dt);
		m_muscles[i]->UpdateInternalForces(dt);
	}
	for (int i = 0; i < m_muscles.size(); i++) {
		m_muscles[i]->UpdateExternalForces(dt);
	}
}

void Creature::UpdateNodes(float dt)
{
	for (int i = 0; i < m_nodes.size(); i++) {
		m_nodes[i]->UpdateExternalForces(dt);
		m_nodes[i]->ApplyForces(dt);
	}
}

float Creature::ComputeFitness()
{
	if (HasLooseNodeGroups()) {
		return -1.f;
	}

	for (int t = 0; t < Config::runTime * Config::runFPS; t++) {
		Update(Config::dt);
		// make sure creature hasnt exploded
	}

	return GetCenter().getLength();
}

Creature Creature::createOffspring()
{
	Creature c = Creature(this);
	c.Mutate();
	return c;
}

void Creature::Mutate()
{
	// small mutation
	auto muscle_rnd = std::uniform_int_distribution<int>(0, int(m_muscles.size())-1);
	int muscle_i = muscle_rnd(m_gen);
	m_muscles[muscle_i]->Mutate(m_gen);

	// big mutation
	auto bigmut_rnd = std::uniform_real_distribution<float>(0.f, 1.f);
	if (bigmut_rnd(m_gen) < Config::bigMutationChance) {
		auto mutType_rnd = std::uniform_int_distribution<int>(0, 3);
		switch (mutType_rnd(m_gen))
		{
		case 0:
			AddRandomNode();
			break;
		case 1:
			AddRandomMuscle();
			break;
		case 2:
			RemoveRandomNode();
			break;
		case 3:
			RemoveRandomMuscle();
			break;
		default:
			break;
		}
	}
}

void Creature::Draw(sf::RenderWindow& window, Vec2f camPos) {
	for (int i = 0; i < m_muscles.size(); i++) {
		m_muscles[i]->Draw(window, camPos);
	}
	for (int i = 0; i < m_nodes.size(); i++) {
		m_nodes[i]->Draw(window, camPos);
	}
}