#include "Game.h"

Game::Game() : window(nullptr), renderer(nullptr), running(false) {}

Game::~Game() { clean(); }

bool Game::init(const char* title, int scr_width, int scr_height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image Init Error: " << IMG_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF Init Error: " << TTF_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scr_width, scr_height, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Load texture bắt buộc
    if (!textureManager.loadTexture("player", "gfx/slime-sheet32px.png", renderer)) return false;
    if (!textureManager.loadTexture("block", "gfx/stoneBlock.png", renderer)) return false;
    if (!textureManager.loadTexture("healbar", "gfx/healbarFull.png", renderer)) return false;
    if (!textureManager.loadTexture("grayheal", "gfx/grayheal.png", renderer)) return false;
    if (!textureManager.loadTexture("expFull", "gfx/expFull.png", renderer)) return false;
    if (!textureManager.loadTexture("exp!Full", "gfx/exp!Full.png", renderer)) return false;
    if (!textureManager.loadTexture("levelup", "gfx/levelup.png", renderer)) return false;
    if (!textureManager.loadTexture("attack", "gfx/attack.png", renderer)) return false;
    if (!textureManager.loadTexture("scarecrow", "gfx/scarecrow.png", renderer)) return false;
    if (!textureManager.loadTexture("slimezombie", "gfx/slimezombie.png", renderer)) return false;
    if (!textureManager.loadTexture("background", "gfx/background.png", renderer)) return false;
    if (!textureManager.loadTexture("pause_icon", "gfx/pause.png", renderer)) return false;
    if (!textureManager.loadTexture("resume_icon", "gfx/resume.png", renderer)) return false;

    // Load texture item (tùy chọn)
    auto loadOptionalTexture = [&](const std::string& id, const std::string& path) {
        if (!textureManager.loadTexture(id, path, renderer)) {
            std::cerr << "Warning: Failed to load optional texture " << path << std::endl;
        }
    };
    loadOptionalTexture("hand", "gfx/hand.png");
    loadOptionalTexture("slime_blob", "gfx/slime_blob.png");
    loadOptionalTexture("slime_boots", "gfx/slime_boots.png");
    loadOptionalTexture("speed_potion", "gfx/speed_potion.png");
    loadOptionalTexture("wood", "gfx/wood.png");
    loadOptionalTexture("stick", "gfx/stick.png");
    loadOptionalTexture("gold", "gfx/gold.png");
    loadOptionalTexture("rope", "gfx/rope.png");
    loadOptionalTexture("stone", "gfx/stone.png");
    loadOptionalTexture("stone_sword", "gfx/stone_sword.png");
    loadOptionalTexture("iron", "gfx/iron.png");
    loadOptionalTexture("iron_sword", "gfx/iron_sword.png");

    lastFrameTime = SDL_GetTicks();
    deltaTime = 0.0f;

    player = new Player(PlayerSpawnX, PlayerSpawnY, 200, 64, 64);

    enemies.push_back(new Scarecrow(2630, 384, player));
    zombies.push_back(new SlimeZombie(3400, 100, 3250, 3550, player));
    for (SlimeZombie* zombie : zombies) {
        zombie->GetTexture(textureManager);
    }

    map = new GameMap();
    if (!map) {
        std::cerr << "Failed to create GameMap!" << std::endl;
        return false;
    }
    map->loadFromFile("txt/map.txt");
    map->loadTextureM(renderer, "block");  // Chỉ load tile texture

    running = true;
    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        player->handleInput(event, *map, enemies, zombies);

    }
}

void Game::resetDeltaTime() {
    lastFrameTime = SDL_GetTicks();
}

void Game::update() {
    Uint32 currentFrameTime = SDL_GetTicks();
    deltaTime = (currentFrameTime - lastFrameTime) / 1000.0f;
    lastFrameTime = currentFrameTime;



    if (player) {

        player->update(deltaTime, *map);
        player->SetMapXY(map->getMap().start_x, map->getMap().start_y);

        for (Enemy* enemy : enemies) {
        enemy->SetMapXY(map->getMap().start_x, map->getMap().start_y);
        enemy->update(deltaTime, *player);
        }

        for (SlimeZombie* zombie : zombies) {
            zombie->SetMapXY(map->getMap().start_x, map->getMap().start_y);
            zombie->GetPlayerXY(player->getHitbox().x + player->getHitbox().w / 2, player->getHitbox().y + player->getHitbox().h / 2);
            zombie->update(deltaTime, *map, *player);
        }

    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Vẽ background
    map->loadbackgroundTexture(renderer, textureManager);
    map->renderBackground(renderer);

    // Vẽ tiles
    map->SetMap(map->getMap());
    map->render(renderer);

    // Vẽ kẻ thù
//std::cout << "Rendering enemies..." << std::endl;
    for (Enemy* enemy : enemies) {
        enemy->render(renderer, textureManager);
    }
    for (SlimeZombie* zombie : zombies) {
        zombie->render(renderer, textureManager);
    }

   // std::cout << "Rendering player..." << std::endl;
    player->SetMapXY(map->getMap().start_x, map->getMap().start_y);
    player->render(renderer, textureManager);

  //  std::cout << "Rendering inventory..." << std::endl;
    player->inventory.render(renderer, textureManager);

   // std::cout << "Presenting..." << std::endl;
    SDL_RenderPresent(renderer);
}
void Game::clean() {
    if (player) {
        player->clean();
        delete player;
        player = nullptr;
    }

    if (map) {
        map->clean();
        delete map;
        map = nullptr;
    }

    for (Enemy* enemy : enemies) {
        delete enemy;
    }
    enemies.clear();

    for (SlimeZombie* zombie : zombies) {
        delete zombie;
    }
    zombies.clear();

    textureManager.cleanTextures();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
    delete player;

}


