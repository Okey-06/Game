#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

using namespace std;

class RenderWindow{
public:
    RenderWindow(const char* p_title, int p_w, int p_h)
        :window(NULL), renderer(NULL){
        window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, p_w, p_h, SDL_WINDOW_SHOWN);

        if(window == NULL){
        cout << "Window failed to init! Error: " << SDL_GetError() << endl;
        }
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }

    SDL_Texture* loadTexture(const char* p_filePath){
        SDL_Texture* texture = NULL;
        texture = IMG_LoadTexture(renderer, p_filePath);

        if(texture == NULL)
            cout << "Failed to load texture! Error: " << SDL_GetError() << endl;

        return texture;
    }

    void cleanup(){
        SDL_DestroyWindow(window);
    }

    void clear(){
        SDL_RenderClear(renderer);
    }

    void render(SDL_Texture* p_tex){
        SDL_RenderCopy(renderer, p_tex, NULL, NULL);
    }

    void display(){
        SDL_RenderPresent(renderer);
    }
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
};

int main(int argc, char* argv[]){
    if(SDL_Init(SDL_INIT_VIDEO) > 0)
        cout << "SDL_Init Failed! SDL_Error: " << SDL_GetError() << endl;

    if(!(IMG_Init(IMG_INIT_PNG)))
        cout << "IMG_Init Failed! Error: " << SDL_GetError() << endl;

    RenderWindow window("Game v1.0", 800, 600);

    SDL_Texture* grassTexture = window.loadTexture("gfx/ground_grass_1.png");



    bool gameRunning = true;

    SDL_Event event;

    while(gameRunning){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT)
                gameRunning = false;
        }

        window.clear();
        window.render(grassTexture);
        window.display();
    }

    window.cleanup();
    SDL_Quit();

    return 0;
}
