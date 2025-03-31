#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "Player.h"
#include "Enemy.h"

class Game {
public:
    Game();
    ~Game();

    bool init(const char* title, int scr_width, int scr_height);
    void handleEvents();
    void update();
    void render();
    void clean();
    void init();

    bool isRunning() const { return running; }

    SDL_Renderer* getRenderer(){return renderer;}
    void resetDeltaTime();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TextureManager textureManager;
    Player* player;
    //Enemy* enemy;
    Uint32 lastFrameTime;
    float deltaTime;

    GameMap* map;

    bool running;

    std::vector<Enemy*> enemies;
    std::vector<SlimeZombie*> zombies;

    SDL_Texture* backgroundTexture;  // Background tĩnh cho map
};

#endif

