#include "app.h"
#include "config.h"
#include <vector>
#include <algorithm>
#include <chrono>

App::App()
{
	Intitialize();
}

void App::Intitialize()
{
	m_creatures.reserve(Config::n_creatures);

	for (int i = 0; i < Config::n_creatures; i++) {
		m_creatures.push_back(std::make_unique<Creature>(Creature(true)));
	}

	// draw
	m_trailShape = sf::CircleShape(2);
	m_trailShape.setPointCount(3);
	m_trailShape.setFillColor(sf::Color::Black);
}

 bool FitnessSortComp(const std::pair<float, int> f0, const std::pair<float, int> f1)
	{
		return (f0.first > f1.first);
	}

void App::RunGeneration()
{
	isRunning = true;
	std::cout << "Running generation\n";
	auto genStart = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < m_creatures.size(); i++) {
		Creature copyCreature = Creature(m_creatures[i].get());

		float fitness = copyCreature.ComputeFitness();
		m_creatures[i].get()->m_fitness = fitness;
	}

	// sort on fitness
	std::vector<std::pair<float, int>> fitness;
	fitness.reserve(m_creatures.size());
	for (int i = 0; i < m_creatures.size(); i++) {
		fitness.push_back(std::pair<float, int >(m_creatures[i].get()->m_fitness, i));
	}

	std::sort(fitness.begin(), fitness.end(), FitnessSortComp);

	float highScore = fitness[0].first;
	m_bestCreature = m_creatures[fitness[0].second].get();

	auto genStop = std::chrono::high_resolution_clock::now();
	auto genDuration = std::chrono::duration_cast<std::chrono::milliseconds>(genStop - genStart);

	std::cout << "Generation finished! Total time: " << genDuration.count() << " ms, max distance of " << highScore << "\n";
	isRunning = false;
}

void App::Run(sf::RenderWindow& window)
{
	RunGeneration();

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
	m_bestCreature->Update(Config::dt);

	Vec2f center = m_bestCreature->GetCenter();
	if (m_trail.empty()) {
		m_trail.push_back(center);
	}
	if (Vec2f::squaredDistance(m_trail.back(), center) > 400.f) {
		m_trail.push_back(center);
	}
}

inline sf::Vector2f toSFVec(Vec2f v) {
	return sf::Vector2f(v.x, v.y);
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

		m_trailShape.setPosition(toSFVec(center - m_trail[i] - 1.f + Vec2f(float(Config::screen_w) / 2.f, float(Config::screen_h) / 2.f)));
		window.draw(m_trailShape);
	}

	m_bestCreature->Draw(window, center - Vec2f(float(Config::screen_w) / 2.f, float(Config::screen_h) / 2.f));
}
