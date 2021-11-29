#include <chrono>
#include <thread>
#include <SFML/Graphics.hpp>
#include "creature.h"

int main()
{
    float targetFPS = 60;
    int targetFrameTimeMs = 1000 / targetFPS;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Waterbug", sf::Style::Default, settings);

    Creature creature = Creature();

    while (window.isOpen())
    {
        auto frameStart = std::chrono::steady_clock::now();

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

        auto frameEnd = std::chrono::steady_clock::now();
        auto frameTime = frameEnd - frameStart;
        while (frameTime < std::chrono::milliseconds(targetFrameTimeMs))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            frameEnd = std::chrono::steady_clock::now();
            frameTime = frameEnd - frameStart;
        }
    }

    return 0;
}