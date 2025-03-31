
#include "Map.h"


GameMap::GameMap() {
    tileTexture = NULL;
    backgroundTexture = NULL;
}

GameMap::~GameMap() {
    if (tileTexture) {
        SDL_DestroyTexture(tileTexture);
    }
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
    }
}

void GameMap::clean() {
    if (tileTexture) {
        SDL_DestroyTexture(tileTexture);
        tileTexture = nullptr;
        std::cout << "✅ Tile texture freed!" << std::endl;
    }
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
        std::cout << "✅ Background texture freed!" << std::endl;
    }
}

std::vector<std::vector<int>>& GameMap::getMapData() {
    return mapData;
}


void GameMap::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Failed to open map file: " << filePath << std::endl;
        return;
    }

    game_map.max_x = 0;
    game_map.max_y = 0;
    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        for (char c : line) {
            row.push_back(c - '0');  // Chuyển ký tự '0', '1' thành số nguyên 0, 1
        }
        mapData.push_back(row);
    }

    //game_map.tile = mapData;

    MAP_W = static_cast<int>(mapData[0].size()) * TILE_SIZE;
    MAP_H = static_cast<int>(mapData.size()) * TILE_SIZE;

    game_map.max_x = MAP_W;
    game_map.max_y = MAP_H;

//    game_map.start_x = 0;
//    game_map.start_y = 0;

    file.close();


     //std::cout << MAP_W << std::endl << MAP_H << std::endl;
}



void GameMap::loadTextureM(SDL_Renderer* renderer, const std::string& id) {
    tileTexture = TextureManager::getTexture(id);
    if (!tileTexture) {
        std::cerr << "Failed to load tile texture!" << std::endl;
    }
}

void GameMap::loadbackgroundTexture(SDL_Renderer* renderer, TextureManager& textureManager){
    backgroundTexture = textureManager.getTexture("background");
    if(!backgroundTexture){
        std::cerr << "Background not load";
    }
}

void GameMap::renderBackground(SDL_Renderer* renderer) {
    SDL_Texture* bgTexture = backgroundTexture;
    if (bgTexture) {
        int bgX = game_map.start_x;
        if (bgX < 0) bgX = 0;
        if (bgX > MAP_W - SCR_W) bgX = MAP_W - SCR_W;
        SDL_Rect bgSrcRect = {bgX, 0, SCR_W, SCR_H};
        SDL_Rect bgDstRect = {0, 0, SCR_W, SCR_H};
        SDL_RenderCopy(renderer, bgTexture, &bgSrcRect, &bgDstRect);
    } else {
        std::cerr << "Background texture is null!" << std::endl;
    }
}

void GameMap::render(SDL_Renderer* renderer) {
    if (!renderer || mapData.empty() || !tileTexture) {
        std::cerr << "Error in GameMap::render!" << std::endl;
        return;
    }

    int map_x = game_map.start_x / TILE_SIZE;
    int map_y = game_map.start_y / TILE_SIZE;
    int x1 = (game_map.start_x % TILE_SIZE) * -1;
    int y1 = (game_map.start_y % TILE_SIZE) * -1;
    int x2 = x1 + SCR_W + (x1 == 0 ? 0 : TILE_SIZE);
    int y2 = y1 + SCR_H + (y1 == 0 ? 0 : TILE_SIZE);

    for (int y = y1, row = map_y; y < y2 && row < static_cast<int>(mapData.size()); y += TILE_SIZE, row++) {
        if (row < 0) continue;
        for (int col = map_x, x = x1; x < x2 && col < static_cast<int>(mapData[row].size()); col++, x += TILE_SIZE) {
            if (col < 0) continue;
            if (mapData[row][col] == 1) {
                SDL_Rect srcRect = {0, 0, TILE_SIZE, TILE_SIZE};
                SDL_Rect destRect = {x, y, TILE_SIZE, TILE_SIZE};
                SDL_RenderCopy(renderer, tileTexture, &srcRect, &destRect);
            }
//            if (mapData[row][col] != 0) {
//                SDL_Rect tileBox = {x, y, TILE_SIZE, TILE_SIZE};
//                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
//                SDL_RenderDrawRect(renderer, &tileBox);
//            }
        }
    }
}
