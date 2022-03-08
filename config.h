#ifndef CONFIG_H_ 
#define CONFIG_H_

#include "Vec2.h"

// APP
#define SCREEN_W 1280
#define SCREEN_H 720
#define SET_RND_SEED true
#define RND_SEED 0

// EVOLUTIONARY ALGORITHM
#define N_CREATURES 100
#define N_GENERATIONS 10
#define RUN_TIME_SECS 10
#define RUN_FPS 60
#define RUN_DT 1.f / float(RUN_FPS)
#define BIG_MUT_P 0.1f

// SIMULATION
#define MAX_VELOCITY 100.f
#define WATER_DRAG_COEF 0.2f
#define WATER_DRAG_COEF_DT 0.9735326f //powf(WATER_DRAG_COEF, RUN_DT)

// CREATURE
#define MAX_SETTLE_ITERATIONS 1000
#define SETTLE_EPSILON 0.0001f
#define SETTLE_FRICTION_COEF 0.8f
#define CREATURE_MIN_NODES 5
#define CREATURE_MAX_NODES 15
#define EDGE_CONNECT_P 0.25f
#define MAX_MUSCLE_LENGTH 80.f
#define MIN_MUSCLE_LENGTH 40.f
#define CLOCK_SPEED 0.5f

#endif  // CONFIG_H_