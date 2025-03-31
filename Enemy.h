#ifndef ENEMY_H
#define ENEMY_H

#include "TextureManager.h"
#include "Map.h"

#define ENEMY_GRAVITY 100
#define ENEMY_FALLMAX 50

class Player;

class Enemy {
protected:
    float x, y;       // Vị trí trên map
    float hp, maxHp;         // Máu của enemy
    SDL_Rect hitbox; // Hitbox để kiểm tra va chạm
    bool isAlive;   // Trạng thái sống/chết
    float respawnTimer; // Thời gian chờ hồi sinh
    float frameTimer;
    int frameCount;

    int frame;
    int frameScarecrowCount = 4;
    float map_x_;
    float map_y_;

    float expReward;  // EXP thưởng khi bị giết

    Player* p;

public:
    Enemy(float x, float y, float maxhealth, Player* player);

    virtual void update(float deltaTime, Player& player);
    virtual void render(SDL_Renderer* renderer, TextureManager& textureManager);
    virtual void takeDamage(float damage);

    void SetMapXY(const int map_x, const int map_y){map_x_ = map_x; map_y_ = map_y;}

    bool checkCollision(SDL_Rect weaponHitbox);

    SDL_Rect getHitbox() const {
        return {hitbox.x - map_x_, hitbox.y, hitbox.w, hitbox.h};
    }
    float getHp(){return hp;}
};

class Scarecrow : public Enemy {
private:

public:
    Scarecrow(float x, float y, Player* player);
    void render(SDL_Renderer* renderer, TextureManager& textureManage) override ;
};

enum State { MOVINGLEFT, MOVINGRIGHT, ATTACKINGLEFT, ATTACKINGRIGHT, DYING, DEAD, IDLE };

class SlimeZombie {
private:
    State currentState;
    float hp, maxHp;
    float damage;

    float attackCooldown;
    float attackCooldownLeft;
    float attackTimer = 2.0f;
    float deathTimer;
    float frameTimer;
    float respawnTimer;
    float attackTime;
    float dyingTime;

    SDL_Rect zb_hitbox;

    bool lastRight;
    bool isAlive;
    bool hasDamage;
    int direction = 1;


    float startX, endX;
    float speed;

    SDL_Texture* zbTexture;

    int frame, frameCount;
    int frameCountMove = 6;
    int frameCountAttack = 8;
    int frameCountIdle = 4;
    int frameCountDead = 3;
    int frameState = 7;

    bool onGround = false;

    float velocityX;
    float velocityY;

    int width = 64;
    int height = 64;
    float x;
    float y;

    float map_x_;
    float map_y_;

    float playerX;
    float playerY;

    Player* p;

    float expReward;  // EXP thưởng khi bị giết
public:

    public:
    SlimeZombie(float spawnX, float spawnY, float start, float end,  Player* p);
    void update(float deltaTime, GameMap& map_data, Player& player);
    void takeDamage(float dmg);
    void render(SDL_Renderer* renderer, TextureManager& textureManager);
    bool checkCollisionVertical(const std::vector<std::vector<int>>& mapData);
    void GetTexture(TextureManager& textureManager);
    void SetMapXY(const int map_x, const int map_y){map_x_ = map_x; map_y_ = map_y;}
    SDL_Rect getZbHitbox() const {
        return {zb_hitbox.x - map_x_, zb_hitbox.y, zb_hitbox.w, zb_hitbox.h};
    }
    void GetPlayerXY(const float player_x, const float player_y) { playerX = player_x; playerY = player_y;}
    float getHp(){return hp;}


};



#endif
