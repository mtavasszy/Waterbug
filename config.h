#ifndef CONFIG_H_ 
#define CONFIG_H_

#include "Vec2.h"

class Config {
public:
	// app
	static constexpr float screen_w = 1280;
	static constexpr float screen_h = 720;
	static constexpr int n_creatures = 100;
	static constexpr int runTime = 15; // seconds
	static constexpr int runFPS = 60;
	static constexpr float dt = 1.f / float(runFPS);

	// creature
	static constexpr int creature_minNodes = 3;
	static constexpr int creature_maxNodes = 5;
	static constexpr float creature_edgeConnectChance = 0.5f;
	static constexpr float creature_maxEdgeLength = 80.f;
	static constexpr float creature_minEdgeLength = 40.f;

	// simulation physics
	static constexpr float waterResistance = 0.3;
	static constexpr float waterFriction = 0.4;
};

#endif  // CONFIG_H_