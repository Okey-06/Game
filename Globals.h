
#ifndef GLOBALS_H
#define GLOBALS_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <utility>
#include <cmath>

extern int MAP_W;
extern int MAP_H;
extern const int TILE_SIZE;

extern SDL_Rect camera;

extern const int SCR_W;
extern const int SCR_H;

//inventory
extern const int slotSize;  // Kích thước mỗi ô
extern const int spacing;   // Khoảng cách giữa các ô
extern const int hotbarSize; //Kích cơ hotbar
extern const int invRows;  // Số hàng của inventory
extern const int invCols;  // Số cột (giống hotbar)


//mouse
extern int mouseX, mouseY;

//healbar && expbar
extern const int barWidth;  // Chiều rộng tối đa của thanh máu
extern const int barHeight;  // Chiều cao thanh máu
extern const int healbarX;     // Vị trí X trên màn hình
extern const int healbarY;     // Vị trí Y trên màn hình
extern const int expX;
extern const int expY;
extern const float MAXHP;

extern const float SCARECROW_EXP_REWARD;  // EXP khi giết Scarecrow
extern const float SLIMEZOMBIE_EXP_REWARD;  // EXP khi giết SlimeZombie

extern const int PlayerSpawnX;
extern const int PlayerSpawnY;

struct Map{
    int start_x;
    int start_y;

    int max_x;
    int max_y;

    int tile[500][100];
    char* file_name;


};



#endif
