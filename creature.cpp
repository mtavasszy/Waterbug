#include "creature.h"
#include "node.h"


Creature::Creature() {
	initialize();
}

void Creature::initialize() {
	bodyNode = Node(0,0,0,0);
	bodyNode.position = sf::Vector2f(1280 / 2, 720 / 2);

	Node secondnode = Node(120, 140, 0.25, 0.25);
	Node thirdnode = Node(240, 280, 0, 0.3);
	bodyNode.addAppendage(secondnode);
	bodyNode.addAppendage(thirdnode);

}

void Creature::simulateStep() {
	bodyNode.simulateStep();
	bodyNode.updatePosition();
}

void Creature::simulate() {

}

void Creature::draw(sf::RenderWindow& window) {
	bodyNode.draw(window, sf::Color::Red);
}