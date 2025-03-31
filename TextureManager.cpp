
#include "TextureManager.h"
//#include "Globals.h"

TextureManager::TextureManager() {
    p_object = NULL;
    rect.x = 0;
    rect.y = 0;
    rect.w = 0;
    rect.h = 0;
}

TextureManager::~TextureManager() {
    cleanTextures();
    //Free();
}

std::map<std::string, SDL_Texture*> TextureManager::textureMap;

//bool TextureManager::LoadImg(std::string path, SDL_Renderer* renderer){
//    SDL_Texture* newTexture = NULL;
//
//    SDL_Surface* loadSurface = IMG_Load(path.c_str());
//    if(loadSurface != NULL){
//        newTexture = SDL_CreateTextureFromSurface(renderer, loadSurface);
//        if(newTexture != NULL){
//            rect.w = loadSurface->w;
//            rect.h = loadSurface->h;
//        }
//    }
//    SDL_FreeSurface(loadSurface);
//    p_object = newTexture;
//    return p_object != NULL;
//}

//void TextureManager::Render(SDL_Renderer* renderer, const SDL_Rect* clip){
//    SDL_Rect renderquad = {rect.x, rect.y, rect.w, rect.h};
//
//    SDL_RenderCopy(renderer, p_object, clip, &renderquad);
//}

//void TextureManager::Free(){
//    if(p_object != NULL){
//        SDL_DestroyTexture(p_object);
//        p_object = NULL;
//        rect.w = 0;
//        rect.h = 0;
//    }
//}


SDL_Texture* TextureManager::getTexture(const std::string& id) {
    if (textureMap.find(id) == textureMap.end()) {
        std::cerr << "Texture not found: " << id << std::endl;
        return nullptr;
    }
    return textureMap[id];
}

bool TextureManager::loadTexture(const std::string& id, const std::string& filePath, SDL_Renderer* renderer) {
    SDL_Surface* tempSurface = IMG_Load(filePath.c_str());
    if (!tempSurface) {
        std::cerr << "Failed to load surface for " << filePath << ": " << IMG_GetError() << std::endl;
        return false;  // Thoát sớm nếu load surface thất bại
    }

    SDL_SetColorKey(tempSurface, SDL_TRUE, SDL_MapRGB(tempSurface->format, 0, 0, 0));
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    if (!texture) {
        std::cerr << "Failed to create texture for " << filePath << ": " << SDL_GetError() << std::endl;
        return false;
    }

    textureMap[id] = texture;
    std::cout << "Loaded texture: " << id << " from " << filePath << std::endl;
    return true;
}

//void TextureManager::drawTexture(const std::string& id, int x, int y, int width, int height, SDL_Renderer* renderer) {
//    SDL_Rect srcRect = { 0, 0, width, height };
//    SDL_Rect destRect = { x, y, width, height };
//
//    SDL_RenderCopy(renderer, textureMap[id], &srcRect, &destRect);
//}

void TextureManager::renderTexture(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int destW, int destH) {
    if (!texture) {
        std::cerr << "Texture is NULL!" << std::endl;
        return;
    }

    int texW, texH;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);  // Lấy kích thước gốc của ảnh
    rect.w = texW;
    rect.h = texH;
    SDL_Rect srcRect = { 0, 0, texW, texH };  // Cắt toàn bộ ảnh
    SDL_Rect destRect = { x, y, destW, destH };  // Render với kích thước mong muốn

    SDL_RenderCopy(renderer, texture, &srcRect, &destRect);
    std::cout << getRect().w;
}


void TextureManager::cleanTextures() {
    for (auto& texture : textureMap) {
        SDL_DestroyTexture(texture.second);
    }
    textureMap.clear();
}
