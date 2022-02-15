#include <SFML/Graphics.hpp>
#include "config.h"
#include "app.h"

int main()
{
	float dt = 1.f / float(Config::runFPS);

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(Config::screen_w, Config::screen_h), "Waterbug", sf::Style::Default, settings);
	window.setFramerateLimit(Config::runFPS);

	App app;
	app.Run(window);

	return 0;
}