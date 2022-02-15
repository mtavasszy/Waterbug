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

bool CreatureSortComp(std::unique_ptr<Creature> c0, std::unique_ptr<Creature> c1) {
	return (c0->m_fitness > c1->m_fitness);
}

void App::RunGeneration()
{
	isRunning = true;
	std::cout << "Running generation\n";
	auto genStart = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < m_creatures.size(); i++) {
		//auto start = std::chrono::high_resolution_clock::now();

		float fitness = RunCreature(i);
		m_creatures[i].get()->m_fitness = fitness;

		//auto stop = std::chrono::high_resolution_clock::now();
		//auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

		//std::cout << "Creature finished in " << duration.count() << "ms with a score of " << fitness << "\n";
	}

	// sort on fitness
	//std::sort(m_creatures.begin(), m_creatures.end(), CreatureSortComp);

	float highScore = 0;
	for (int i = 0; i < m_creatures.size(); i++) {
		if (m_creatures[i]->m_fitness > highScore) {
			highScore = m_creatures[i]->m_fitness;
			m_bestCreature = m_creatures[i].get();
		}
	}

	auto genStop = std::chrono::high_resolution_clock::now();
	auto genDuration = std::chrono::duration_cast<std::chrono::milliseconds>(genStop - genStart);

	std::cout << "Generation finished! Total time: " << genDuration.count() << " ms, max distance of " << highScore << "\n";
	isRunning = false;
}

float App::RunCreature(int i)
{
	Creature creature = Creature(m_creatures[i].get());

	for (int t = 0; t < Config::runTime * Config::runFPS; t++) {
		creature.Update(Config::dt);
		// make sure creature hasnt exploded
	}

	return creature.GetCenter().getLength();
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
