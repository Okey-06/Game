#include "Game.h"

// Định nghĩa biến toàn cục từ Globals.h
std::vector<Mix_Music*> musicPlaylist;
int currentMusicIndex = 0;
int musicVolume = 64; // Âm lượng mặc định (50% của 128)
Mix_Chunk* jumpSound = nullptr; // Khởi tạo âm thanh nhảy

Game::Game() : window(nullptr), renderer(nullptr), running(false) {}

Game::~Game() { clean(); }

bool Game::init(const char* title, int scr_width, int scr_height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL Mixer Init Error: " << Mix_GetError() << std::endl;
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

    // Load font để hiển thị tên bài nhạc
//    font = TTF_OpenFont("txt/FontPixel.ttf", 24); // Dùng font hiện có trong dự án
//    if (!font) {
//        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
//        return false;
//    }

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
    if (!textureManager.loadTexture("speaker_on", "gfx/music_on.png", renderer)) return false;
    if (!textureManager.loadTexture("speaker_off", "gfx/music_off.png", renderer)) return false;

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

// Tải danh sách nhạc
    std::vector<std::string> musicFiles = {
        "sfx/dark.mp3",
        "sfx/phaos.mp3", // Thêm các tệp nhạc của bạn vào đây
        "sfx/bacbling.mp3",
        "sfx/tanthe.mp3"
    };
    loadMusicPlaylist(musicFiles);
    setMusicVolume(musicVolume); // Đặt âm lượng mặc định
    playNextMusic(); // Phát bài đầu tiên

    // Tải âm thanh nhảy
    loadJumpSound("sfx/jumpsound.wav");
    // Lưu con trỏ Game để dùng trong callback
    SDL_SetWindowData(window, "game", this);

    lastFrameTime = SDL_GetTicks();
    deltaTime = 0.0f;

    player = new Player(PlayerSpawnX, PlayerSpawnY, 200, 64, 64);
    player->setGame(this); // Truyền con trỏ Game vào Player

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

        if (event.type == SDL_KEYDOWN) {
            // Tạm dừng/tiếp tục nhạc bằng phím M
            if (event.key.keysym.sym == SDLK_m) {
                if (Mix_PausedMusic()) {
                    resumeMusic();
                } else if (Mix_PlayingMusic()) {
                    pauseMusic();
                }
            }
            // Chuyển bài tiếp theo bằng phím N
            if (event.key.keysym.sym == SDLK_n) {
                playNextMusic();
            }
            // Điều chỉnh âm lượng bằng phím + và -
            if (event.key.keysym.sym == SDLK_EQUALS || event.key.keysym.sym == SDLK_PLUS) {
                setMusicVolume(musicVolume + 10);
            }
            if (event.key.keysym.sym == SDLK_MINUS) {
                setMusicVolume(musicVolume - 10);
            }
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

    renderSpeakerIcon(); // Vẽ biểu tượng loa

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

// Giải phóng danh sách nhạc
    for (Mix_Music* music : musicPlaylist) {
        Mix_FreeMusic(music);
    }
    musicPlaylist.clear();
    Mix_HookMusicFinished(nullptr); // Xóa callback

    textureManager.cleanTextures();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
    delete player;
    exit(0);    // Đảm bảo thoát hoàn toàn

}

void Game::loadMusicPlaylist(const std::vector<std::string>& filePaths) {
    for (const auto& path : filePaths) {
        Mix_Music* music = Mix_LoadMUS(path.c_str());
        if (!music) {
            std::cerr << "Failed to load music: " << path << " - " << Mix_GetError() << std::endl;
        } else {
            musicPlaylist.push_back(music);
            std::cout << "✅ Loaded music: " << path << std::endl;
        }
    }
    if (musicPlaylist.empty()) {
        std::cerr << "Warning: No music files loaded!" << std::endl;
    }
}

void Game::playNextMusic() {
    if (musicPlaylist.empty()) return;

    // Dừng bài nhạc hiện tại nếu đang phát
    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }

    // Tăng chỉ số và quay vòng nếu cần
    currentMusicIndex = (currentMusicIndex + 1) % musicPlaylist.size();

    // Phát bài nhạc tiếp theo
    if (Mix_PlayMusic(musicPlaylist[currentMusicIndex], 0) == -1) { // 0 để phát một lần
        std::cerr << "Failed to play music: " << Mix_GetError() << std::endl;
    } else {
        isMusicPlaying = true;
    }

    // Đăng ký callback để tự động chuyển bài khi nhạc kết thúc
    Mix_HookMusicFinished([]() {
        Game* game = static_cast<Game*>(SDL_GetWindowData(SDL_GetKeyboardFocus(), "game"));
        if (game) {
            game->playNextMusic(); // Gọi lại để lặp vòng
        }
    });
}

void Game::pauseMusic() {
    if (Mix_PlayingMusic() && !Mix_PausedMusic()) {
        Mix_PauseMusic();
        isMusicPlaying = false; // Cập nhật trạng thái loa
        std::cout << "⏸️ Music paused." << std::endl;
    }
}

void Game::resumeMusic() {
    if (Mix_PausedMusic()) {
        Mix_ResumeMusic();
        isMusicPlaying = true; // Cập nhật trạng thái loa
        std::cout << "▶️ Music resumed." << std::endl;
    }
}

void Game::setMusicVolume(int volume) {
    if (volume < 0) volume = 0;
    if (volume > MIX_MAX_VOLUME) volume = MIX_MAX_VOLUME; // MIX_MAX_VOLUME = 128
    musicVolume = volume;
    Mix_VolumeMusic(musicVolume);
    std::cout << "🔊 Music volume set to " << musicVolume << " (" << (musicVolume * 100 / MIX_MAX_VOLUME) << "%)" << std::endl;
}

void Game::renderSpeakerIcon() {
    SDL_Texture* speakerTexture = isMusicPlaying ? textureManager.getTexture("speaker_on") : textureManager.getTexture("speaker_off");
    if (!speakerTexture) {
        std::cerr << "Speaker texture not found!" << std::endl;
        return;
    }

    // Đặt vị trí và kích thước cho biểu tượng loa (góc trên phải, dưới tên bài nhạc)
    int speakerSize = 32; // Kích thước biểu tượng (có thể điều chỉnh)
    int speakerX = SCR_W - speakerSize - 10;
    int speakerY = 50; // Dưới tên bài nhạc
    SDL_Rect speakerRect = {speakerX, speakerY, speakerSize, speakerSize};
    SDL_RenderCopy(renderer, speakerTexture, NULL, &speakerRect);
}

void Game::loadJumpSound(const std::string& filePath) {
    jumpSound = Mix_LoadWAV(filePath.c_str());
    if (!jumpSound) {
        std::cerr << "Failed to load jump sound: " << Mix_GetError() << std::endl;
    } else {
        std::cout << "✅ Jump sound loaded: " << filePath << std::endl;
    }
}
void Game::playJumpSound() {
    if (jumpSound) {
        if (Mix_PlayChannel(-1, jumpSound, 0) == -1) {
            std::cerr << "Failed to play jump sound: " << Mix_GetError() << std::endl;
        } else {
            std::cout << "🔊 Jump sound played!" << std::endl;
        }
    }
}
