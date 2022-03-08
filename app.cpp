#include "app.h"
#include "config.h"
#include <vector>
#include <algorithm>
#include <chrono>
#include "utils.h"

App::App()
{
	std::random_device rd;
	m_gen = std::mt19937(rd());
	if (SET_RND_SEED) {
		m_gen.seed(RND_SEED);
		std::cout << "App seed set to " << RND_SEED << "\n";
	}
}

void App::IntitializeCreatures()
{
	std::cout << "Generating " << N_CREATURES << " random creatures.\n";
	auto genStart = std::chrono::high_resolution_clock::now();

	auto dis_seed = std::uniform_int_distribution<unsigned int>(0, UINT32_MAX);

	m_creatures.reserve(N_CREATURES);

#pragma omp parallel for
	for (int i = 0; i < N_CREATURES; i++) {
		m_creatures.push_back(std::make_unique<Creature>(Creature(true, dis_seed(m_gen))));
		if (i % (N_CREATURES / 25) == 0)
			std::cout << "|";
	}
	std::cout << "\n";

	// draw
	m_trailShape = sf::CircleShape(2);
	m_trailShape.setPointCount(3);
	m_trailShape.setFillColor(sf::Color::Black);

	auto genStop = std::chrono::high_resolution_clock::now();
	auto genDuration = std::chrono::duration_cast<std::chrono::milliseconds>(genStop - genStart);

	std::cout << "Generation finished! Total time: " << genDuration.count() << " ms\n";
}

bool FitnessSortComp(const std::pair<float, int> f0, const std::pair<float, int> f1)
{
	return (f0.first > f1.first);
}

void App::RunGeneration()
{
#pragma omp parallel for
	for (int i = 0; i < m_creatures.size(); i++) {
		Creature copyCreature = Creature(m_creatures[i].get());

		float fitness = copyCreature.ComputeFitness();
		m_creatures[i].get()->m_fitness = fitness;
	}
}

void App::CreateOffspring()
{
	// sort on fitness
	std::vector<std::pair<float, int>> fitnessRanking;
	fitnessRanking.reserve(m_creatures.size());
	for (int i = 0; i < m_creatures.size(); i++) {
		fitnessRanking.push_back(std::pair<float, int >(m_creatures[i].get()->m_fitness, i));
	}
	std::sort(fitnessRanking.begin(), fitnessRanking.end(), FitnessSortComp);

	// let top half reproduce
	std::vector<std::unique_ptr<Creature>> offspring(m_creatures.size());

#pragma omp parallel for
	for (int i = 0; i < fitnessRanking.size() / 2; i++) {
		int c_i = fitnessRanking[i].second;
		Creature* c = m_creatures[c_i].get();

		offspring[i * 2] = std::make_unique<Creature>(c->createOffspring());
		offspring[i * 2 + 1] = std::make_unique<Creature>(c->createOffspring());
	}

	float highScore = fitnessRanking[0].first;

	if (m_bestCreature == nullptr || highScore > m_bestCreature->m_fitness)
		m_bestCreature = std::make_unique<Creature>(Creature(m_creatures[fitnessRanking[0].second].get()));

	std::cout << "Creature " << fitnessRanking[0].second << " got the highest distance of " << highScore << "\n";

	m_creatures.clear();
	m_creatures = std::move(offspring);
}

void App::RunMultipleGenerations()
{
	for (int i = 0; i < N_GENERATIONS; i++) {
		std::cout << "Running gen " << i << "\n";
		auto genStart = std::chrono::high_resolution_clock::now();

		RunGeneration();

		auto genStop = std::chrono::high_resolution_clock::now();
		auto genDuration = std::chrono::duration_cast<std::chrono::milliseconds>(genStop - genStart);
		std::cout << "Generation finished! Total time: " << genDuration.count() << " ms\n";

		genStart = std::chrono::high_resolution_clock::now();

		CreateOffspring();

		genStop = std::chrono::high_resolution_clock::now();
		genDuration = std::chrono::duration_cast<std::chrono::milliseconds>(genStop - genStart);
		std::cout << "Selection finished! Total time: " << genDuration.count() << " ms\n";

	}

	if (PAUSE_AFTER_FINISH)
		std::system("pause");
}

void App::Run(sf::RenderWindow& window)
{
	IntitializeCreatures();
	RunMultipleGenerations();

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear(sf::Color(14, 135, 204, 255));
		Update();
		Draw(window);
		window.display();
	}
}

void App::Update()
{
	m_bestCreature->Update();

	Vec2f center = m_bestCreature->GetCenter();
	if (m_trail.empty()) {
		m_trail.push_back(center);
	}
	if (Vec2f::squaredDistance(m_trail.back(), center) > 400.f) {
		m_trail.push_back(center);
	}
}

void App::Draw(sf::RenderWindow& window)
{
	Vec2f center = m_bestCreature->GetCenter();

	for (int i = 0; i < m_trail.size(); i++) {
		if (i == 0) {
			m_trailShape.setFillColor(sf::Color::White);
		}
		else {
			m_trailShape.setFillColor(sf::Color::Black);
		}

		m_trailShape.setPosition(Utils::toSFVec(m_trail[i] - 1.f + Vec2f(float(SCREEN_W) / 2.f, float(SCREEN_H) / 2.f) - center));
		window.draw(m_trailShape);
	}

	m_bestCreature->Draw(window, center - Vec2f(float(SCREEN_W) / 2.f, float(SCREEN_H) / 2.f));
}
