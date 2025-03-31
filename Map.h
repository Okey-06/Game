#ifndef MAP_G
#define MAP_G

#include "Globals.h"
#include "TextureManager.h"

#define Max_Tiles 20


class GameMap {
private:
    std::vector<std::vector<int>> mapData;
    SDL_Texture* tileTexture;
    SDL_Texture* backgroundTexture;
    Map game_map;
//    TileMat tile_mat[Max_Tiles];

public:
    GameMap();
    ~GameMap();

    void clean();
    void loadFromFile(const std::string& filePath);
    void loadTextureM(SDL_Renderer* renderer, const std::string& id);
    void loadbackgroundTexture(SDL_Renderer* renderer, TextureManager& textureManager);
    void renderBackground(SDL_Renderer* renderer);
    void render(SDL_Renderer* renderer);
    std::vector<std::vector<int>>& getMapData();
    void SetMap(Map& map_data){game_map = map_data;}
    Map& getMap(){return game_map;}




};

#endif

