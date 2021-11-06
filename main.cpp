#include <SFML/Graphics.hpp>
#include "creature.h"

int main()
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Waterbug", sf::Style::Default, settings);

    Creature creature = Creature();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color(14, 135, 204,255));
        creature.simulateStep();
        creature.draw(window);
        window.display();
    }

    return 0;
}