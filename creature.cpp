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

	float genArea = 3.14f * Config::creature_maxEdgeLength * Config::creature_maxEdgeLength * nNodes;
	float genSideLength = sqrtf(genArea) * 0.5f;

	// add nodes
	for (int i = 0; i < nNodes; i++) {
		AddRandomNode(genSideLength);
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
	for (int i = 0; i < m_muscles.size(); i++) {
		if (IsCrossingMuscle(m_muscles[i]->m_Ai, m_muscles[i]->m_Bi))
			return true;
	}

	return false;
}

bool Creature::IsCrossingMuscle(int A_i, int B_i)
{
	const Vec2f p1 = m_nodes[A_i]->m_position;
	const Vec2f q1 = m_nodes[B_i]->m_position;

	for (int i = 0; i < m_muscles.size(); i++) {
		Muscle* m = m_muscles[i].get();
		if (!m->ContainsNode(A_i) && !m->ContainsNode(B_i)) {
			const Vec2f p2 = m->m_nodeA->m_position;
			const Vec2f q2 = m->m_nodeB->m_position;

			if (Vec2f::isIntersect(p1, q1, p2, q2))
				return true;
		}
	}

	return false;
}

void Creature::AddRandomNode(float genAreaLength)
{
	auto dis_pos = std::uniform_real_distribution<float>(-genAreaLength, genAreaLength);
	auto dis_norm = std::uniform_real_distribution<float>(0.f, 1.f);

	Vec2f pos = Vec2f(dis_pos(m_gen), dis_pos(m_gen));
	m_nodes.push_back(std::make_unique<Node>(Node(pos)));

	// connect muscles
	if (m_nodes.size() > 1) {
		for (int i = 0; i < m_nodes.size() - 1; i++) {
			if (dis_norm(m_gen) < Config::creature_edgeConnectChance) {
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
				m_muscles[i]->HandleDelete();
				m_muscles.erase(m_muscles.begin() + i);
				i--;
			}
		}

		// remove node
		m_nodes.erase(m_nodes.begin() + n_i);

		// fix connectedNodes
		for (int i = 0; i < m_nodes.size(); i++) {
			Node* n = m_nodes[i].get();
			for (int j = 0; j < n->m_connectedNodes.size(); j++) {
				if (n->m_connectedNodes[j] > n_i)
					n->m_connectedNodes[j]--;
			}
		}

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
		m_muscles[m_i]->HandleDelete();
		m_muscles.erase(m_muscles.begin() + m_i);
	}
}

void Creature::Update(float dt) {
	SetHull();

	for (int i = 0; i < m_nodes.size(); i++) {
		m_nodes[i]->ResetForces();
	}

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

void Creature::SetHull()
{
	// reset muscles
	for (int i = 0; i < m_muscles.size(); i++) {
		m_muscles[i]->m_isHullAB = false;
		m_muscles[i]->m_isHullBA = false;
	}

	// get leftmost node
	int currNode = 0;
	for (int i = 1; i < m_nodes.size(); i++) {
		if (m_nodes[i]->m_position.x < m_nodes[currNode]->m_position.x) {
			currNode = i;
		}
	}

	Vec2f prevVec = Vec2f(-1.f, 0.f);
	int nextNode;

	for (int i = 0; i < 2 * m_nodes.size(); i++) {
		std::vector<int> connectedNodes = m_nodes[currNode]->m_connectedNodes;

		if (connectedNodes.empty()) {
			return;
		}

		nextNode = connectedNodes.front();
		float smallestCWAngle = 360.f;

		for (int j = 0; j < connectedNodes.size(); j++) {
			Vec2f jVec = Vec2f(m_nodes[connectedNodes[j]]->m_position - m_nodes[currNode]->m_position).normalize();
			float angle = Vec2f::getClockWiseAngle(prevVec, jVec);
			if (angle < smallestCWAngle) {
				smallestCWAngle = angle;
				nextNode = connectedNodes[j];
			}
		}

		int muscle = GetMuscle(currNode, nextNode);
		if (muscle != -1) {
			if (currNode == m_muscles[muscle]->m_Ai) {
				if (m_muscles[muscle]->m_isHullAB == true) {
					return;
				}
				m_muscles[muscle]->m_isHullAB = true;

			}
			else {
				if (m_muscles[muscle]->m_isHullBA == true) {
					return;
				}
				m_muscles[muscle]->m_isHullBA = true;

			}
		}

		prevVec = Vec2f(m_nodes[currNode]->m_position - m_nodes[nextNode]->m_position).normalize();;
		currNode = nextNode;
	}
}

void Creature::UpdateMuscles(float dt)
{
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
		case 0: {
			float genArea = 3.14f * Config::creature_maxEdgeLength * Config::creature_maxEdgeLength * (m_nodes.size() + 1);
			float genSideLength = sqrtf(genArea) * 0.5f;
			AddRandomNode(genSideLength);
			break;
		}
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