#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "Globals.h"

class TextureManager {
public:
    TextureManager();
    ~TextureManager();

    static SDL_Texture* getTexture(const std::string& id);
    bool loadTexture(const std::string& id, const std::string& filePath, SDL_Renderer* renderer);
//    void drawTexture(const std::string& id, int x, int y, int width, int height, SDL_Renderer* renderer);
    void cleanTextures();
    //static void loadTexture(const std::string& id, const char* filePath, SDL_Renderer* renderer);
    //static void clean();

    void renderTexture(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int destW, int destH);
    //void SetRect(const int& x, const int& y){rect.x = x; rect.y = y;}
    //SDL_Rect GetRect() const {return rect;}
    //SDL_Texture* GetTexture() const {return p_object;}

    //bool LoadImg(std::string path, SDL_Renderer* renderer);
    //void Render(SDL_Renderer* renderer, const SDL_Rect* clip = NULL);
    //void Free();

    SDL_Renderer* getRenderer(){return renderer;}
    SDL_Rect getRect() const {return {rect.x, rect.y, rect.w, rect.h};}


private:
    static std::map<std::string, SDL_Texture*> textureMap;
    //static std::map<std::string, SDL_Texture*> textures;
    SDL_Texture* p_object;
    SDL_Renderer* renderer;
    SDL_Rect rect;
};

#endif

