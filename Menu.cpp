
#include "Menu.h"

Menu::Menu(SDL_Renderer* renderer) : renderer(renderer), font(NULL), startTexture(NULL), exitTexture(NULL), backgroundTexture(NULL) {
    font = TTF_OpenFont("txt/FontPixel.ttf", 24);
    if (!font) {
        std::cerr << "Không thể load font: " << TTF_GetError() << std::endl;
    }
    textColor = {255, 255, 255};  // Màu trắng

    // Load background
    SDL_Surface* bgSurface = IMG_Load("gfx/menu.png");
    if (bgSurface) {
        backgroundTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
        SDL_FreeSurface(bgSurface);
        if (!backgroundTexture) {
            std::cerr << "Error: Failed to create background texture: " << SDL_GetError() << std::endl;
        }
    } else {
        std::cerr << "Error: Failed to load menu_background.png: " << IMG_GetError() << std::endl;
    }

    // Tạo texture cho text
    startTexture = createTextTexture("START GAME");
    exitTexture = createTextTexture("EXIT");

    // Định nghĩa vùng click cho các nút
    int w, h;
    SDL_QueryTexture(startTexture, NULL, NULL, &w, &h);
    startRect = { 250, SCR_H / 2 - 160, 700, 100};

    SDL_QueryTexture(exitTexture, NULL, NULL, &w, &h);
    exitRect = {SCR_W / 2 - w - 10, SCR_H / 2 - 20 , 100, 50};
}

Menu::~Menu() {
    if (font) TTF_CloseFont(font);
    if (startTexture) SDL_DestroyTexture(startTexture);
    if (exitTexture) SDL_DestroyTexture(exitTexture);
    if (backgroundTexture) SDL_DestroyTexture(backgroundTexture);
    TTF_Quit();
}

SDL_Texture* Menu::createTextTexture(const std::string& text) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    if (!surface) return NULL;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

bool Menu::showMenu() {
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) return false;

            // Xử lý click chuột
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                // Kiểm tra click vào nút Start
                if (mouseX >= startRect.x && mouseX <= startRect.x + startRect.w &&
                    mouseY >= startRect.y && mouseY <= startRect.y + startRect.h) {
                    return true;  // Bắt đầu game
                }

                // Kiểm tra click vào nút Exit
                if (mouseX >= exitRect.x && mouseX <= exitRect.x + exitRect.w &&
                    mouseY >= exitRect.y && mouseY <= exitRect.y + exitRect.h) {
                    return false;  // Thoát game
                }
            }

            // Xử lý phím
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RETURN) return true;  // Bắt đầu game
                if (event.key.keysym.sym == SDLK_ESCAPE) return false; // Thoát game
            }
        }

        // Render background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (backgroundTexture) {
            SDL_Rect bgRect = {0, 0, SCR_W, SCR_H};
            SDL_RenderCopy(renderer, backgroundTexture, NULL, &bgRect);
        } else {
            std::cerr << "Warning: Background texture not loaded!" << std::endl;
        }

        // Render các nút
        //SDL_RenderCopy(renderer, startTexture, NULL, &startRect);
        //SDL_RenderCopy(renderer, exitTexture, NULL, &exitRect);

        SDL_RenderPresent(renderer);
    }

    return false;
}
