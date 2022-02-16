#ifndef CONFIG_H_ 
#define CONFIG_H_

#include "Vec2.h"

class Config {
public:
	// app
	static constexpr int screen_w = 1280;
	static constexpr int screen_h = 720;
	static constexpr int n_creatures = 100;
	static constexpr int n_gens = 100;
	static constexpr int runTime = 30; // seconds
	static constexpr int runFPS = 60;
	static constexpr float dt = 1.f / float(runFPS);

	// creature
	static constexpr int creature_minNodes = 3;
	static constexpr int creature_maxNodes = 6;
	static constexpr float creature_edgeConnectChance = 0.35f;
	static constexpr float creature_maxEdgeLength = 80.f;
	static constexpr float creature_minEdgeLength = 40.f;

	// simulation physics
	static constexpr float waterDragCoef = 0.3f;
	static constexpr float waterFrictionCoef = 0.5f;

	// evolution
	static constexpr float bigMutationChance = 0.1f;
};

#endif  // CONFIG_H_