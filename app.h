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
	float RunCreature(int i);

	void Run(sf::RenderWindow& window);
	void Draw(sf::RenderWindow& window);

	std::vector<std::unique_ptr<Creature>> m_creatures;
	bool isRunning = false;

	Creature* m_bestCreature;
};

#endif  // APP_H_