#include "Globals.h"

int MAP_W = 0;
int MAP_H = 0;
const int TILE_SIZE = 32;

SDL_Rect camera;


const int SCR_W = 1200;
const int SCR_H = 600;

const int slotSize = 50;
const int spacing = 10;
const int invRows = 2;
const int invCols = 5;
const int hotbarSize = 5;

const int barWidth = 200;
const int barHeight = 50;
const int healbarX = 20;
const int healbarY = 20;
const int expX = 50;
const int expY = 80;
const float MAXHP = 100;

int mouseX;
int mouseY;

const float SCARECROW_EXP_REWARD = 5;
const float SLIMEZOMBIE_EXP_REWARD = 150;

const int PlayerSpawnX = 200;
const int PlayerSpawnY = 300;
