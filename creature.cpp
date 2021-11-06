#include "creature.h"
//#include "edge.h"
#include "node.h"


Creature::Creature() {
	bodyNode = Node(sf::Vector2f(1280/2, 720/2));

	Node secondnode = Node(sf::Vector2f(1280 / 2 - 50, 720 / 2 - 30));
	Node thirdnode = Node(sf::Vector2f(1280 / 2 + 50, 720 / 2 - 30));
	bodyNode.appendages.push_back(secondnode);
	bodyNode.appendages.push_back(thirdnode);
}

void Creature::initialize() {


}

void Creature::simulateStep() {
	// moving water takes energy
	// moving other appendages costs energy

	for (Node node : bodyNode.appendages) {
		//ap.simulateStep();
	}
}

void Creature::simulate() {

}

void Creature::draw(sf::RenderWindow& window) {
	bodyNode.draw(window, sf::Color::Red);
}