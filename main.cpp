#include <SFML/Graphics.hpp>
#include "creature.h"
#include "Vec2.h"
#include <vector>

int main()
{
	int targetFPS = 60;
	float dt = 1.f / float(targetFPS);

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(1280, 720), "Waterbug", sf::Style::Default, settings);
	window.setFramerateLimit(targetFPS);

	std::vector<Creature> creatures;
	for (int x = 0; x < 6; x++) {
		for (int y = 0; y < 3; y++) {
			Vec2f pos = Vec2f(x * 200 + 50, y * 200 + 50);
			creatures.push_back(Creature(true, pos));
		}
	}
	for (auto it = creatures.begin(); it != creatures.end(); it++) {
		it->update(dt);
	}
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear(sf::Color(14, 135, 204, 255));
		for (auto it = creatures.begin(); it != creatures.end(); it++) {
			it->update(dt);
			it->draw(window);
		}
		window.display();
	}

	return 0;
}