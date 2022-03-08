#include <SFML/Graphics.hpp>
#include "config.h"
#include "app.h"

int main()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(SCREEN_W, SCREEN_H), "Waterbug", sf::Style::Default, settings);
	window.setFramerateLimit(RUN_FPS);

	App app;
	app.Run(window);

	return 0;
}