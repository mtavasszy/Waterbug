#include "creature.h"
#include "config.h"
#include "node.h"
#include "Vec2.h"
#include <queue>
#include <set>
#include "utils.h"

Creature::Creature(bool init) {

	// random
	std::random_device rd;
	m_gen = std::mt19937(rd()); // Standard mersenne_twister_engine seeded with rd()
	m_fitness = 0.f;

	// init creature
	if (init) {
		do {
			GenerateRandom();

			CheckIfValidCreature();
			TrySettleStructure();
		} while (!m_isValid || !m_isSettled);
	}
}

Creature::Creature(const Creature* c)
{
	m_gen = c->m_gen;

	for (int i = 0; i < c->m_nodes.size(); i++) {
		m_nodes.push_back(std::make_unique<Node>(Node(c->m_nodes[i].get())));
	}
	for (int i = 0; i < c->m_muscles.size(); i++) {
		m_muscles.push_back(std::make_unique<Muscle>(Muscle(c->m_muscles[i].get())));
		m_muscles[i]->ResetNodePointers(this);
	}

	m_isValid = c->m_isValid;
	m_isSettled = c->m_isSettled;

	m_fitness = 0.f;
}

void Creature::GenerateRandom()
{
	m_nodes.clear();
	m_muscles.clear();

	auto dis_nodes = std::uniform_int_distribution<int>(Config::creature_minNodes, Config::creature_maxNodes);
	int nNodes = dis_nodes(m_gen);

	// add nodes
	for (int i = 0; i < nNodes; i++) {
		AddRandomNode();
	}
}

void Creature::TrySettleStructure()
{
	for (int i = 0; i < Config::creature_settleIterations; i++) {
		Update(Config::dt);
	}

	if (IsExploded()) {
		m_isSettled = false;
		return;
	}

	ReCenter();
	m_isSettled = true;
}

void Creature::ReCenter()
{
	Vec2f center = GetCenter();
	for (int i = 0; i < m_nodes.size(); i++) {
		m_nodes[i]->m_position -= center;
		m_nodes[i]->m_velocity = Vec2f(0.f);
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

void Creature::CheckIfValidCreature()
{
	m_isValid = !HasLooseNodeGroups() && !HasCrossingMuscles();
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

bool Creature::HasCrossingMuscles()
{
	return false;
}

bool Creature::IsCrossingMuscle()
{
	return false;
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

				m_muscles.push_back(std::make_unique<Muscle>(Muscle(this, n0_i, n1_i, dis_norm(m_gen), dis_norm(m_gen))));
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
				if ((m_muscles[i]->m_Ai == n0_i && m_muscles[i]->m_Bi == n1_i) || (m_muscles[i]->m_Ai == n1_i && m_muscles[i]->m_Bi == n0_i)) {
					muscleExists = true;
					break;
				}
			}
		} while (muscleExists);

		m_muscles.push_back(std::make_unique<Muscle>(Muscle(this, n0_i, n1_i, dis_norm(m_gen), dis_norm(m_gen))));
	}
}

void Creature::RemoveRandomNode()
{
	if (m_nodes.size() > Config::creature_minNodes && m_muscles.size() >= m_nodes.size()) {
		auto node_rnd = std::uniform_int_distribution<int>(0, int(m_nodes.size()) - 1);
		int n_i = node_rnd(m_gen);

		// remove connecting muscles
		for (int i = 0; i < m_muscles.size(); i++) {
			if (m_muscles[i]->m_Ai == n_i || m_muscles[i]->m_Bi == n_i) {
				m_muscles.erase(m_muscles.begin() + i);
				i--;
			}
		}

		// remove node
		m_nodes.erase(m_nodes.begin() + n_i);

		// fix muscle node pointers
		for (int i = 0; i < m_muscles.size(); i++) {
			if (m_muscles[i]->m_Ai >= n_i) {
				m_muscles[i]->m_Ai--;
			}
			if (m_muscles[i]->m_Bi >= n_i) {
				m_muscles[i]->m_Bi--;
			}
			m_muscles[i]->ResetNodePointers(this);
		}
	}
}

void Creature::RemoveRandomMuscle()
{
	if (m_muscles.size() > m_nodes.size()) {
		auto muscle_rnd = std::uniform_int_distribution<int>(0, int(m_muscles.size()) - 1);
		int m_i = muscle_rnd(m_gen);
		m_muscles.erase(m_muscles.begin() + m_i);
	}
}

void Creature::Update(float dt) {
	UpdateMuscles(dt);
	UpdateNodes(dt);
}

int Creature::GetMuscle(int A, int B)
{
	for (int i = 0; i < m_muscles.size(); i++) {
		if ((A == m_muscles[i]->m_Ai && B == m_muscles[i]->m_Bi) || (A == m_muscles[i]->m_Bi && B == m_muscles[i]->m_Ai))
			return i;
	}
	return -1;
}

int orientation(Vec2f p, Vec2f q, Vec2f r)
{
	int val = int((q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y));
	if (val == 0) return 0; 	 // colinear
	return (val > 0) ? 1 : 2; 	// clock or counterclock wise
}

void Creature::SetHull()
{
	// reset muscles
	for (int i = 0; i < m_muscles.size(); i++) {
		m_muscles[i]->m_isHull = false;
	}

	// get leftmost node
	int currNode = 0;
	for (int i = 1; i < m_nodes.size(); i++) {
		if (m_nodes[i]->m_position.x < m_nodes[currNode]->m_position.x) {
			currNode = i;
		}
	}

	int nextNode;

	for (int i = 0; i < 2 * m_nodes.size(); i++) {
		nextNode = (currNode + 1) % m_nodes.size();
		for (int j = 0; j < m_nodes.size(); j++) {
			if (j!=currNode && orientation(m_nodes[currNode]->m_position, m_nodes[j]->m_position, m_nodes[nextNode]->m_position) == 2)
				nextNode = j;
		}

		int muscle = GetMuscle(currNode, nextNode);
		if (muscle != -1) 
			m_muscles[muscle]->m_isHull = true;
		
		currNode = nextNode;
	}
}

void Creature::UpdateMuscles(float dt)
{
	//SetHull();
	for (int i = 0; i < m_muscles.size(); i++) {
		m_muscles[i]->UpdateClock(dt);
		m_muscles[i]->SetNormal();
		m_muscles[i]->UpdateInternalForces(dt);
	}
	for (int i = 0; i < m_muscles.size(); i++) {
		m_muscles[i]->UpdateExternalForces(dt);
	}
}

void Creature::UpdateNodes(float dt)
{
	for (int i = 0; i < m_nodes.size(); i++) {
		m_nodes[i]->ApplyForces(dt);
	}
}

float Creature::ComputeFitness()
{
	if (!m_isValid || !m_isSettled)
		return -1.f;

	const int nSteps = Config::runTime * Config::runFPS;
	for (int t = 0; t < nSteps; t++) {
		Update(Config::dt);
	}

	if (IsExploded())
		return -1.f;

	return GetCenter().getLength();
}

bool Creature::IsExploded()
{
	float maxSpeed = Config::creature_maxEdgeLength * 5;
	float maxSpeedSqr = maxSpeed * maxSpeed;

	for (int i = 0; i < m_nodes.size(); i++) {
		if (m_nodes[i]->m_velocity.getSquaredLength() > maxSpeedSqr)
			return true;
	}
	for (int i = 0; i < m_muscles.size(); i++) {
		if (Vec2f::squaredDistance(m_muscles[i]->m_nodeA->m_position, m_muscles[i]->m_nodeB->m_position) > maxSpeedSqr)
			return true;
	}

	return false;
}

Creature Creature::createOffspring()
{
	Creature c = Creature(this);
	std::random_device rd;
	c.m_gen = std::mt19937(rd());
	c.Mutate();

	// validate creature
	c.CheckIfValidCreature();
	c.TrySettleStructure();

	return c;
}

void Creature::Mutate()
{
	// small mutation
	auto muscle_rnd = std::uniform_int_distribution<int>(0, int(m_muscles.size()) - 1);
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