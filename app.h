#ifndef APP_H_ 
#define APP_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include "creature.h"

#include "Vec2.h"

class App {
public:
	App();

	void Intitialize();
	void RunGeneration();
	void CreateOffspring();
	void RunMultipleGenerations();

	void Run(sf::RenderWindow& window);
	void Update();
	void Draw(sf::RenderWindow& window);

	std::vector<std::unique_ptr<Creature>> m_creatures;

	std::unique_ptr<Creature> m_bestCreature;
	std::vector<Vec2f> m_trail;
	sf::CircleShape m_trailShape;
};

#endif  // APP_H_