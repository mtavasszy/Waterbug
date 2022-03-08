#include "creature.h"

#define _USE_MATH_DEFINES
#include <queue>
#include <set>
#include <math.h>
#include "config.h"
#include "node.h"
#include "Vec2.h"
#include "utils.h"

Creature::Creature(bool init, unsigned int seed) {

	// random
	std::random_device rd;
	m_gen = std::mt19937(rd());
	m_gen.seed(seed);

	m_fitness = 0.f;

	// init creature
	if (init) {
		do {
			GenerateRandom();
			m_isSettling = true;
			TrySettleStructure();
		} while (!m_isValid);

		SetHull();
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
	m_isSettling = false;

	m_fitness = 0.f;
}

void Creature::GenerateRandom()
{
	m_nodes.clear();
	m_muscles.clear();

	auto dis_nodes = std::uniform_int_distribution<int>(CREATURE_MIN_NODES, CREATURE_MAX_NODES);
	auto dis_angle = std::uniform_real_distribution<float>(0.f, 2 * float(M_PI));
	auto dis_param = std::uniform_real_distribution<float>(0.f, 1.f);

	int nNodes = dis_nodes(m_gen);

	m_nodes.push_back(std::make_unique<Node>(Node(Vec2f(0.f))));
	m_nodes.push_back(std::make_unique<Node>(Node(Vec2f(cos(dis_angle(m_gen)), sin(dis_angle(m_gen))) * MAX_MUSCLE_LENGTH)));

	m_muscles.push_back(std::make_unique<Muscle>(Muscle(this, 0, 1, dis_param(m_gen), dis_param(m_gen))));

	// add nodes
	for (int i = 2; i < nNodes; i++) {
		AddRandomNode();
	}
}

void Creature::TrySettleStructure()
{
	m_isSettling = true;

	for (int i = 0; i < MAX_SETTLE_ITERATIONS; i++) {
		Update();

		if (i > 20 && IsSettled()) {
			break;
		}
	}

	SetValidCreature();

	if (!m_isValid) {
		return;
	}

	ReCenter();
	m_isSettling = false;
}

bool Creature::IsSettled()
{
	for (int i = 0; i < m_nodes.size(); i++) {
		if (m_nodes[i]->m_velocity.getSquaredLength() > SETTLE_EPSILON)
			return false;
	}
	return true;
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

void Creature::SetValidCreature()
{
	m_isValid = !HasLooseNodeGroups() && !HasCrossingMuscles() && !IsExploded();
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

			if (Utils::isIntersect(p1, q1, p2, q2))
				return true;
		}
	}

	return false;
}

bool Creature::IsCrossingMuscle(int A_i, Vec2f q1)
{
	const Vec2f p1 = m_nodes[A_i]->m_position;

	for (int i = 0; i < m_muscles.size(); i++) {
		if (!m_muscles[i]->ContainsNode(A_i)) {
			const Vec2f p2 = m_muscles[i]->m_nodeA->m_position;
			const Vec2f q2 = m_muscles[i]->m_nodeB->m_position;

			if (Utils::isIntersect(p1, q1, p2, q2))
				return true;
		}
	}

	return false;
}

void Creature::AddRandomNode()
{
	float nodeDistSqr = MAX_MUSCLE_LENGTH * MAX_MUSCLE_LENGTH;
	float maxNodeDistSqr = nodeDistSqr * 2.25f;

	auto dis_node = std::uniform_int_distribution<int>(0, int(m_nodes.size()) - 1);
	auto dis_connect = std::uniform_int_distribution<int>(0, 1);

	auto dis_param = std::uniform_real_distribution<float>(0.f, 1.f);

	Vec2f newNodePos;
	int newNode_i = int(m_nodes.size());

	int connectedNode_i;
	bool connectedIsCrossing = true;

	int maxTries = 100;
	int tries = 0;

	do {
		int firstNode_i;
		int secondNode_i;

		// pick random node
		firstNode_i = dis_node(m_gen);

		// pick random node that is close to firstnode
		std::vector<int> closeNodes;

		for (int i = 0; i < m_nodes.size(); i++) {
			if (i != firstNode_i && Vec2f::squaredDistance(m_nodes[firstNode_i]->m_position, m_nodes[i]->m_position) < maxNodeDistSqr) {
				closeNodes.push_back(i);
			}
		}

		if (closeNodes.empty())
			continue;

		auto dis_secondNode = std::uniform_int_distribution<int>(0, int(closeNodes.size()) - 1);
		secondNode_i = closeNodes[dis_secondNode(m_gen)];

		// determine best of two possible positions
		Vec2f v = m_nodes[secondNode_i]->m_position - m_nodes[firstNode_i]->m_position;
		float dist = Vec2f::distance(m_nodes[secondNode_i]->m_position, m_nodes[firstNode_i]->m_position);
		Vec2f v_norm = v / dist;
		Vec2f v_ort = Vec2f::getOrthogonal(v_norm);

		Vec2f posUp = m_nodes[firstNode_i]->m_position + v_norm * MAX_MUSCLE_LENGTH * 0.5f + v_ort * MAX_MUSCLE_LENGTH;
		Vec2f posDown = m_nodes[firstNode_i]->m_position + v_norm * MAX_MUSCLE_LENGTH * 0.5f - v_ort * MAX_MUSCLE_LENGTH;

		float minSqrDistUp = FLT_MAX;
		float minSqrDistDown = FLT_MAX;

		for (int i = 0; i < m_nodes.size(); i++) {
			if (i != firstNode_i && i != secondNode_i) {
				minSqrDistUp = std::min(minSqrDistUp, Vec2f::squaredDistance(posUp, m_nodes[i]->m_position));
				minSqrDistDown = std::min(minSqrDistDown, Vec2f::squaredDistance(posDown, m_nodes[i]->m_position));
			}
		}

		if (minSqrDistUp < nodeDistSqr) {
			if (minSqrDistDown < nodeDistSqr) {
				continue;
			}
			else {
				newNodePos = posDown;
			}
		}
		else {
			if (minSqrDistDown < nodeDistSqr) {
				newNodePos = posUp;
			}
			else {
				newNodePos = minSqrDistUp > minSqrDistDown ? posUp : posDown;
			}
		}

		connectedNode_i = dis_connect(m_gen) == 1 ? firstNode_i : secondNode_i;

		connectedIsCrossing = IsCrossingMuscle(connectedNode_i, newNodePos);
		if (connectedIsCrossing) {
			connectedNode_i = connectedNode_i == firstNode_i ? secondNode_i : firstNode_i;
			connectedIsCrossing = IsCrossingMuscle(connectedNode_i, newNodePos);
		}

		tries++;

	} while (connectedIsCrossing && tries <= maxTries);

	m_nodes.push_back(std::make_unique<Node>(Node(newNodePos)));
	m_muscles.push_back(std::make_unique<Muscle>(Muscle(this, connectedNode_i, newNode_i, dis_param(m_gen), dis_param(m_gen))));

	// connect muscles
	for (int i = 0; i < m_nodes.size() - 2; i++) {
		float sqrDist = Vec2f::squaredDistance(newNodePos, m_nodes[i]->m_position);

		if (i != connectedNode_i
			&& sqrDist < maxNodeDistSqr
			&& dis_param(m_gen) < EDGE_CONNECT_P
			&& !IsCrossingMuscle(i, newNode_i)) {
			m_muscles.push_back(std::make_unique<Muscle>(Muscle(this, i, newNode_i, dis_param(m_gen), dis_param(m_gen))));
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
	if (m_nodes.size() > CREATURE_MIN_NODES && m_muscles.size() >= m_nodes.size()) {
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

void Creature::Update() {

	for (int i = 0; i < m_nodes.size(); i++) {
		m_nodes[i]->ResetForces();
	}

	UpdateMuscles();
	UpdateNodes();
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
	if (!m_isValid)
		return;

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
		if (m_nodes[currNode]->m_connectedNodes.empty()) {
			return;
		}

		nextNode = m_nodes[currNode]->m_connectedNodes.front();
		float smallestCWAngle = 360.f;

		for (int j = 0; j < m_nodes[currNode]->m_connectedNodes.size(); j++) {
			int jNode = m_nodes[currNode]->m_connectedNodes[j];
			Vec2f jVec = Vec2f(m_nodes[jNode]->m_position - m_nodes[currNode]->m_position).normalize();
			float angle = Utils::getClockWiseAngle(prevVec, jVec);
			if (angle < smallestCWAngle) {
				smallestCWAngle = angle;
				nextNode = jNode;
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

void Creature::UpdateMuscles()
{
	for (int i = 0; i < m_muscles.size(); i++) {
		m_muscles[i]->UpdateClock();

		if (m_isSettling) {
			m_muscles[i]->m_restLength = m_muscles[i]->m_expandLength;
		}

		m_muscles[i]->SetNormal();
		m_muscles[i]->UpdateInternalForces();
	}
}

void Creature::UpdateNodes()
{
	for (int i = 0; i < m_nodes.size(); i++) {
		m_nodes[i]->ApplyDrag(this, i);
		m_nodes[i]->ApplyForces();
		m_nodes[i]->CorrectCollisions(this, i);

		if (m_isSettling) {
			m_nodes[i]->m_velocity *= SETTLE_FRICTION_COEF;
		}
	}
}

float Creature::ComputeFitness()
{
	SetValidCreature();

	if (!m_isValid)
		return -1.f;

	const int nSteps = RUN_TIME_SECS * RUN_FPS;
	for (int t = 0; t < nSteps; t++) {
		Update();
	}

	SetValidCreature();

	if (!m_isValid)
		return -1.f;

	return GetCenter().getLength();
}

bool Creature::IsExploded()
{
	float maxSpeed = MAX_MUSCLE_LENGTH * 5;
	float maxSpeedSqr = maxSpeed * maxSpeed;

	for (int i = 0; i < m_nodes.size(); i++) {
		if (!isnan(m_nodes[i]->m_position.x) && !isnan(m_nodes[i]->m_position.y) && m_nodes[i]->m_velocity.getSquaredLength() > maxSpeedSqr)
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
	c.TrySettleStructure();

	c.SetHull();

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
	if (bigmut_rnd(m_gen) < BIG_MUT_P) {
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