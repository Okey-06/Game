#include "Game.h"
#include "Globals.h"
#include "Menu.h"

int main(int argc, char* argv[]) {
    srand(time(0));
    Game game;

    if (!game.init("Slime Phiêu lưu ký", SCR_W, SCR_H)) {
        return -1;
    }

    Menu menu(game.getRenderer());

    bool inMenu = true;
    while (inMenu) {
    if (!menu.showMenu()) {
        game.clean();
        return 0;
        }
    inMenu = false;
    }

    game.resetDeltaTime();

    while (game.isRunning()) {
        game.handleEvents();
        game.update();
        game.render();
    }

    game.clean();

    return 0;
}
