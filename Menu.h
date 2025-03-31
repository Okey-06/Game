#ifndef MENU_H
#define MENU_H

#include "Globals.h"

class Menu {
public:
    Menu(SDL_Renderer* renderer);
    ~Menu();

    bool showMenu();  // Hiển thị menu, trả về true nếu chọn "Bắt đầu", false nếu thoát

    TTF_Font* getFont() { return font; }

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Color textColor;
    SDL_Texture* startTexture;
    SDL_Texture* exitTexture;
    SDL_Texture* backgroundTexture;  // Texture cho background

    SDL_Rect startRect;  // Vùng của nút Start
    SDL_Rect exitRect;   // Vùng của nút Exit

    SDL_Texture* createTextTexture(const std::string& text);
};

#endif
