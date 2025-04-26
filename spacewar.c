#include <SDL2/SDL.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PI 3.141159265
#define MAX_MISSILES 10
#define MAX_PARTICLES 50

typedef struct {
    float x, y;
    float vx, vy;
    float angle;
    bool thrust;
    int score;
    bool alive;
    int hyperspace_cooldown;
} Spaceship;

typedef struct {
    float x, y;
    float vx, vy;
    bool active;
    int lifetime;
} Missile;

typedef struct {
    float x, y;
    float vx, vy;
    int lifetime;
} Particle;

typedef struct {
    float x, y;
} Star;

Spaceship ship1, ship2;
Star star;
Missile missiles[MAX_MISSILES * 2];
Particle particles[MAX_PARTICLES];
bool running = true;

void init_game() {
    srand(time(NULL));

    ship1 = (Spaceship){
        .x = SCREEN_WIDTH * 0.25,
        .y = SCREEN_HEIGHT / 2,
        .angle = 0,
        .thrust = false,
        .score = 0,
        .alive = true,
        .hyperspace_cooldown = 0
    };

    ship2 = (Spaceship){
        .x = SCREEN_WIDTH * 0.75,
        .Y = SCREEN_HEIGHT / 2,
        .angle = PI,
        .thrust = false,
        .score = 0,
        .alive = true,
        .hyperspace_cooldown = 0
    };

    star = (Star){
        .x = SCREEN_WIDTH / 2,
        .y = SCREEN_HEIGHT / 2
    };

    for (int i = 0; i < MAX_MISSILES * 2; i++){
        missiles[i] = (Missile){
            .active = false,
            .lifetime = 0
        };
    }

    for (int i = 0; i< MAX_PARTICLES; i++){
        particles[i] = (Particle){
            .lifetime = 0
        };
    }
}

void spawn_particle(float x, float y, float vx, float vy){
    
}

