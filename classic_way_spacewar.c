#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h> // Para usleep()

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 24
#define PI 3.14159265
#define GRAVITY 1000

typedef struct {
    int x, y;       // Posição (coordenadas inteiras)
    int vx, vy;     // Velocidade (fix-point)
    int angle;      // Ângulo (0-359 graus)
    bool thrust;    // Propulsão ativa?
    bool alive;     // Nave destruída?
    int score;      // Pontuação
} Spaceship;

typedef struct {
    int x, y;       // Posição
    bool active;    // Míssil ativo?
} Missile;

typedef struct {
    int x, y;       // Estrela central
} Star;

// Variáveis globais (como no código original)
Spaceship ship1, ship2;
Star star;
Missile missiles[10];
bool running = true;

// Inicializa o jogo
void init_game() {
    ship1 = (Spaceship){
        .x = SCREEN_WIDTH / 4,
        .y = SCREEN_HEIGHT / 2,
        .angle = 0,
        .thrust = false,
        .alive = true,
        .score = 0
    };

    ship2 = (Spaceship){
        .x = 3 * SCREEN_WIDTH / 4,
        .y = SCREEN_HEIGHT / 2,
        .angle = 180,
        .thrust = false,
        .alive = true,
        .score = 0
    };

    star = (Star){
        .x = SCREEN_WIDTH / 2,
        .y = SCREEN_HEIGHT / 2
    };

    for (int i = 0; i < 10; i++) {
        missiles[i] = (Missile){
            .active = false
        };
    }
}

// Desenha o frame ASCII
void draw_frame() {
    char screen[SCREEN_HEIGHT][SCREEN_WIDTH + 1];

    // Limpa a tela (preenche com espaços)
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            screen[y][x] = ' ';
        }
        screen[y][SCREEN_WIDTH] = '\0';
    }

    // Desenha a estrela central (*)
    screen[star.y][star.x] = '*';

    // Desenha a nave 1 (A)
    if (ship1.alive) {
        screen[ship1.y][ship1.x] = 'A';
    }

    // Desenha a nave 2 (V)
    if (ship2.alive) {
        screen[ship2.y][ship2.x] = 'V';
    }

    // Desenha mísseis (.)
    for (int i = 0; i < 10; i++) {
        if (missiles[i].active) {
            screen[missiles[i].y][missiles[i].x] = '.';
        }
    }

    // Exibe a tela
    printf("\033[H"); // Move o cursor para o canto
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        printf("%s\n", screen[y]);
    }

    // Mostra a pontuação
    printf("Jogador 1: %d | Jogador 2: %d\n", ship1.score, ship2.score);
}

// Atualiza física da nave (usando inteiros para simular fix-point)
void update_ship(Spaceship *ship) {
    if (!ship->alive) return;

    // Gravidade da estrela (lei do inverso do quadrado)
    int dx = star.x - ship->x;
    int dy = star.y - ship->y;
    int dist_sq = dx * dx + dy * dy;
    if (dist_sq > 0) {
        ship->vx += (dx * GRAVITY) / dist_sq;
        ship->vy += (dy * GRAVITY) / dist_sq;
    }

    // Propulsão (converte ângulo para vetor)
    if (ship->thrust) {
        ship->vx += (int)(10 * cos(ship->angle * PI / 180));
        ship->vy += (int)(10 * sin(ship->angle * PI / 180));
    }

    // Atualiza posição
    ship->x += ship->vx / 100;
    ship->y += ship->vy / 100;

    // Wrap-around (se sair da tela)
    if (ship->x < 0) ship->x = SCREEN_WIDTH - 1;
    if (ship->x >= SCREEN_WIDTH) ship->x = 0;
    if (ship->y < 0) ship->y = SCREEN_HEIGHT - 1;
    if (ship->y >= SCREEN_HEIGHT) ship->y = 0;
}

// Dispara um míssil
void fire_missile(Spaceship *ship, int player_id) {
    for (int i = player_id * 5; i < (player_id + 1) * 5; i++) {
        if (!missiles[i].active) {
            missiles[i] = (Missile){
                .x = ship->x,
                .y = ship->y,
                .active = true
            };
            break;
        }
    }
}

// Atualiza mísseis
void update_missiles() {
    for (int i = 0; i < 10; i++) {
        if (missiles[i].active) {
            // Move o míssil (direção fixa por simplicidade)
            missiles[i].x += (i < 5) ? 1 : -1;

            // Verifica colisão com naves
            Spaceship *target = (i < 5) ? &ship2 : &ship1;
            if (target->alive && 
                abs(missiles[i].x - target->x) < 2 && 
                abs(missiles[i].y - target->y) < 2) {
                target->alive = false;
                missiles[i].active = false;
                if (i < 5) ship1.score++;
                else ship2.score++;
            }

            // Desativa se sair da tela
            if (missiles[i].x < 0 || missiles[i].x >= SCREEN_WIDTH) {
                missiles[i].active = false;
            }
        }
    }
}

// Teletransporte aleatório (hyperspace)
void hyperspace(Spaceship *ship) {
    ship->x = rand() % SCREEN_WIDTH;
    ship->y = rand() % SCREEN_HEIGHT;
    ship->vx = ship->vy = 0;
}

int main() {
    srand(time(NULL));
    init_game();

    while (running) {
        // Lógica de controle (teclado via stdin)
        char input = 0;
        if (system("stty -echo") == -1) {} // Desativa eco do teclado (Unix)
        if (system("stty cbreak") == -1) {} // Lê teclas sem Enter

        struct timeval tv;
        fd_set fds;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
            input = getchar();
        }

        // Jogador 1: WASD + F para disparar, Q para hyperspace
        ship1.thrust = (input == 'w');
        if (input == 'a') ship1.angle = (ship1.angle - 10) % 360;
        if (input == 'd') ship1.angle = (ship1.angle + 10) % 360;
        if (input == 'f') fire_missile(&ship1, 0);
        if (input == 'q') hyperspace(&ship1);

        // Jogador 2: IJKL + H para disparar, U para hyperspace
        ship2.thrust = (input == 'i');
        if (input == 'j') ship2.angle = (ship2.angle - 10) % 360;
        if (input == 'l') ship2.angle = (ship2.angle + 10) % 360;
        if (input == 'h') fire_missile(&ship2, 1);
        if (input == 'u') hyperspace(&ship2);

        // Atualiza física
        update_ship(&ship1);
        update_ship(&ship2);
        update_missiles();

        // Renderização
        draw_frame();

        // Delay para ~30 FPS
        usleep(33000);
    }

    return 0;
}