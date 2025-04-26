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
    for (int i = 0; i< MAX_PARTICLES; i++){
        if (particles[i].lifetime <= 0){
            particles[i] = (Particle){
                .x = x,
                .y = y,
                .vx = vx + ((rand() % 100) - 50) * 0.01,
                .vy = vy + ((rand() % 100) - 50) * 0.01,
                .lifetime = 20 + (rand() % 30)
            };
            break;
        }
    }
}

void fire_missile(Spaceship *ship, int player_id){
    for( int i = player_id * MAX_MISSILES; i < (player_id + 1) * MAX_MISSILES; i++) {
        if (!missiles[i].active) {
            missiles[i] = (Missile) {
                .x = ship->x,
                .y = ship->y,
                .vx = ship->vx + cos(ship->angle) * 5,
                .vy = ship->vy + sin(ship->angle) * 5,
                .active = true,
                .lifetime = 120
            };
            break;
        }
    }
}

void hyperspace(Spaceship *ship) {
    if (ship->hyperspace_cooldown <= 0){
        ship->x = rand() % SCREEN_WIDTH;
        ship->y = rand() % SCREEN_HEIGHT;
        ship->vx = 0;
        ship->vy = 0;
        ship->hyperspace_cooldown = 180;
    }
}

void update_ship(Spaceship *ship) {
    if (!ship->alive) return;

    float dx = star.x - ship->x;
    float dy = star.y - ship->y;
    float dist = sqrt(dx * dx + dy *dy);
    float force = 1000.0f / (dist * dist);

    ship->vx += dx / dist * force * 0.01;
    ship->vy += dy / dist * force * 0.01;

    if (ship->thrust) {
        ship->vx += cos(ship->angle) * 0.2;
        ship->vy += sin(ship->angle) * 0.2;
        spawn_particle(
            ship->x - cos(ship->angle) * 10,
            ship->y - sin(ship->angle) * 10,
            -cos(ship->angle) * 0.5,
            -sin(ship->angle) * 0.5
        );
    }

    ship->x += ship->vx;
    ship->y += ship->vy;

    if (ship->x < 0) ship->x = SCREEN_WIDTH;
    if (ship->x > SCREEN_WIDTH) ship->x = 0;
    if (ship->y < 0) ship->y = SCREEN_HEIGHT;
    if (ship->y < 0) ship->y = ship->y = 0;

    if (ship->hyperspace_cooldown > 0) {
        ship->hyperspace_cooldown--;
    }
}

void update_missiles() {
    for(int i =0; i< MAX_MISSILES * 2; i++) {
        if(missiles[i].active) {
            missiles[i].x += missiles[i].vx;
            missiles[i].y += missiles[i].vy;
            missiles[i].lifetime--;

            if (missiles[i].x < 0) missiles[i].x = SCREEN_WIDTH;
            if (missiles[i].x > SCREEN_WIDTH) missiles[i].x = 0;
            if (missiles[i].y < 0) missiles[i].y = SCREEN_HEIGHT;
            if (missiles[i].y > SCREEN_HEIGHT) missiles[i].y = 0;

            if(missiles[i].active) {
                Spaceship *target = (i < MAX_MISSILES) ? &ship2 : &ship1;

                if (target->alive){
                    float dist = sqrt(
                        pow(missiles[i].x - target->x, 2) +
                        pow(missiles[i].y - target->y, 2)
                    );
                    if (dist < 15){
                        target->alive = false;
                        missiles[i].active = false;
                        if (i< MAX_MISSILES) ship1.SCORE++;
                        else ship2.score++
                    }
                    
                }
                
            }

            if (missiles[i].lifetime <= 0){
                missiles[i].active = false;
            }
            
        }
    }
}

void update_particles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].lifetime > 0){
            particles[i].x += particles[i].vx;
            particles[i].y += particles[i].vy;
            particles[i].lifetime--;
        }
        
    }
}

void draw_ship(SDL_Renderer *renderer, Spaceship *ship) {
    if (!ship->alive) return;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    float x1 = ship->x + cos(ship->angle) * 10;
    float y1 = ship->y + sin(ship->angle) * 10;
    float x2 = ship->x + cos(ship->angle + 2.5) * 7;
    float y2 = ship->y + sin(ship->angle + 2.5) * 7;
    float x3 = ship->x + cos(ship->angle - 2.5) * 7;
    float y3 = ship->y + sin(ship->angle - 2.5) * 7;

    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
    SDL_RenderDrawLine(renderer, x3, y3, x1, y1);
}

void draw_missiles(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < MAX_MISSILES * 2; i++) {
        if (missiles[i].active) {
            SDL_RenderDrawPoint(renderer, missiles[i].x, missiles[i].y);
        }
    }
}

void draw_particles(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 150, 0, 255);
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].lifetime > 0) {
            SDL_RenderDrawPoint(renderer, particles[i].x, particles[i].y);
        }
    }
}

void draw_score(SDL_Renderer *renderer) {
    char score_text[50];
    sprintf(score_text, "Jogador 1: %d | Jogador 2: %d", ship1.score, ship2.score);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surface = SDL_CreateRGBSurface(0, 300, 30, 32, 0, 0, 0, 0);
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
    // Nota: Para texto, você precisaria de SDL_ttf ou desenhar manualmente.
    // Aqui é um placeholder. Instale SDL_ttf para texto real.
    SDL_Rect score_rect = {10, 10, 300, 30};
    SDL_BlitSurface(surface, NULL, SDL_GetWindowSurface(window), &score_rect);
    SDL_FreeSurface(surface);
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow(
        "Spacewar! (MIT 1962 - C)", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        SCREEN_WIDTH, 
        SCREEN_HEIGHT, 
        0
    );
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    init_game();

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;

            const Uint8 *keys = SDL_GetKeyboardState(NULL);
            
            ship1.thrust = keys[SDL_SCANCODE_SPACE];
            if (keys[SDL_SCANCODE_A]) ship1.angle -= 0.05;
            if (keys[SDL_SCANCODE_D]) ship1.angle += 0.05;
            if (keys[SDL_SCANCODE_W] && !missiles[0].active) fire_missile(&ship1, 0);
            if (keys[SDL_SCANCODE_Q]) hyperspace(&ship1);

            ship2.thrust = keys[SDL_SCANCODE_RETURN];
            if (keys[SDL_SCANCODE_LEFT]) ship2.angle -= 0.05;
            if (keys[SDL_SCANCODE_RIGHT]) ship2.angle += 0.05;
            if (keys[SDL_SCANCODE_UP] && !missiles[MAX_MISSILES].active) fire_missile(&ship2, 1);
            if (keys[SDL_SCANCODE_DOWN]) hyperspace(&ship2);
        }

        // Atualiza física
        update_ship(&ship1);
        update_ship(&ship2);
        update_missiles();
        update_particles();

        // Renderização
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Estrela
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderDrawPoint(renderer, star.x, star.y);

        // Partículas
        draw_particles(renderer);

        // Mísseis
        draw_missiles(renderer);

        // Naves
        draw_ship(renderer, &ship1);
        draw_ship(renderer, &ship2);

        // Pontuação
        draw_score(renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}