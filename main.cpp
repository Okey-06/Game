#include <SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TILE_SIZE = 40;
const int WALL_THICKNESS = 1; // Độ dày của tường
const int MAP_ROWS = SCREEN_HEIGHT / TILE_SIZE;
const int MAP_COLS = SCREEN_WIDTH / TILE_SIZE;
const int TANK_SPEED = 1; // Giảm tốc độ xe
const int BULLET_SPEED = 1;

int map[MAP_ROWS][MAP_COLS];

struct Bullet {
    int x, y;
    int speed = BULLET_SPEED;
    Bullet(int startX, int startY) : x(startX), y(startY) {}
    void move() { y -= speed; }
    void render(SDL_Renderer* renderer) {
        SDL_Rect bulletRect = {x, y, 5, 10};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &bulletRect);
    }
};

struct Tank {
    int x, y;
    int size = 30;
    std::vector<Bullet> bullets;
    Tank(int startX, int startY) : x(startX), y(startY) {}

    bool canMove(int newX, int newY) {
        int left = newX / TILE_SIZE;
        int right = (newX + size - 1) / TILE_SIZE;
        int top = newY / TILE_SIZE;
        int bottom = (newY + size - 1) / TILE_SIZE;

        return (map[top][left] == 0 && map[top][right] == 0 &&
                map[bottom][left] == 0 && map[bottom][right] == 0);
    }

    void move(int dx, int dy) {
        int newX = x + dx * TANK_SPEED;
        int newY = y + dy * TANK_SPEED;
        if (canMove(newX, newY)) {
            x = newX;
            y = newY;
        }
    }

    void shoot() {
        bullets.push_back(Bullet(x + size / 2, y));
    }

    void updateBullets() {
        for (auto& bullet : bullets) {
            bullet.move();
        }
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet& b) { return b.y < 0; }), bullets.end());
    }

    void render(SDL_Renderer* renderer) {
        SDL_Rect tankRect = {x, y, size, size};
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &tankRect);
        for (auto& bullet : bullets) {
            bullet.render(renderer);
        }
    }
};

Tank playerTank(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

void initMap() {
    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            if (row < WALL_THICKNESS || row >= MAP_ROWS - WALL_THICKNESS || col < WALL_THICKNESS || col >= MAP_COLS - WALL_THICKNESS) {
                map[row][col] = 1;
            } else if ((row % 3 == 0 && col % 3 == 0) && !(row < 2 && col < 2)) {
                map[row][col] = 2;
            } else {
                map[row][col] = 0;
            }
        }
    }
}

void drawMap(SDL_Renderer* renderer) {
    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            SDL_Rect tile = {col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            if (map[row][col] == 1) {
                SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
                SDL_RenderFillRect(renderer, &tile);
            } else if (map[row][col] == 2) {
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
                SDL_RenderFillRect(renderer, &tile);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL không thể khởi tạo! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Battle City SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Không thể tạo cửa sổ! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    initMap();

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                playerTank.shoot();
            }
        }

        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_UP]) playerTank.move(0, -1);
        if (state[SDL_SCANCODE_DOWN]) playerTank.move(0, 1);
        if (state[SDL_SCANCODE_LEFT]) playerTank.move(-1, 0);
        if (state[SDL_SCANCODE_RIGHT]) playerTank.move(1, 0);

        playerTank.updateBullets();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        drawMap(renderer);
        playerTank.render(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
