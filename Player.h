
#ifndef PLAYER_H
#define PLAYER_H

#include "Map.h"
#include "Inventory.h"
#include "Enemy.h"

class Game;

class Player {
public:
    Player(int x, int y, int speed, int p_width, int p_height);
    void handleInput(const SDL_Event& event, GameMap& map_data,
                    std::vector<Enemy*>& enemies, std::vector<SlimeZombie*>& zombies);
    void update(float deltaTime, GameMap& map_data);
    void render(SDL_Renderer* renderer, TextureManager& textureManager);
    bool checkCollisionVertical(const std::vector<std::vector<int>>& mapData, bool checkTop);
    bool checkCollisionHorizontal(const std::vector<std::vector<int>>& mapData);
    bool checkCollision(const SDL_Rect& other);
    bool checkHead(const std::vector<std::vector<int>>& mapData);


    enum PlayerState { IDLE, RUNNING_LEFT, RUNNING_RIGHT, JUMPING, FALLING, PUNCHING, SWORD_ATTACK, AXE_ATTACK };

    float getX();
    float getY();
    SDL_Rect getHitbox();
    SDL_Rect getHitboxWeapon();
    const SDL_Rect& getHitboxP() const { return hitbox; }

    void SetMapXY(const int map_x, const int map_y){map_x_ = map_x; map_y_ = map_y;}
    //std::cout << "SetMapXY: map_x_ = " << map_x_ << ", map_y_ = " << map_y_ << std::endl;}
    void CenterPlayerOnMap(Map& map_data);

    ~Player() { clean(); }
    void clean();

    void takeDamage(float dmg);
    void addExp(float exp);
    void attack(std::vector<Enemy*>& enemies, std::vector<SlimeZombie*>& zombies);

    Inventory inventory;

    void renderHealthBar(SDL_Renderer* renderer, TextureManager& textureManager);
    void renderExpBar(SDL_Renderer* renderer, TextureManager& textureManager);
    void renderLevelup(SDL_Renderer* renderer, TextureManager& textureManager);
    void renderLevel(SDL_Renderer* renderer);
    void renderAttack(SDL_Renderer* renderer, TextureManager& textureManager);
    bool checkAttackCollision(std::vector<Enemy*>& enemies, std::vector<SlimeZombie*>& zombies);
    void getTilePosition(int& row, int& col);  // Lấy chỉ số ô trong mapData
    void tryPickupItem(const SDL_Event& event, GameMap& map_data);  // Xử lý nhặt item
    void tryOpenCraftingMenu(const SDL_Event& event, GameMap& map_data);  // Mở menu crafting
    void useItem();  // Hàm để dùng item trong hotbar
    void resetPlayer();  // Hàm reset player khi chết
    bool GetisPaused() const { return isPaused; }  // Kiểm tra game có đang tạm dừng không
    bool GetisGameOver() const { return gameOver; }  // Kiểm tra game over
    void setGame(Game* g) { game = g; } // Setter cho con trỏ Game

private:
    float x, y;
    float speed;
    float baseSpeed;      // Giá trị gốc của speed (để reset)
    float velocityX, velocityY;
    float width, height;
    float p_width, p_height;

    // Animation
    int frame;          // Chỉ số frame hiện tại
    int frameCount;     // Tổng số frame trong sprite sheet
    int frameCountState = 8;     // số trạng thái
    float frameTimer;
    float attackTime;    // Bộ đếm thời gian frame
    int frameAttackCount;
    int frameAttack;


    float gravity = 100.0f;  // Lực hấp dẫn
    bool onGround = false;   // Kiểm tra nhân vật có đang trên mặt đất không

    float jumpForce = -20.0f;  // Lực nhảy (âm vì hướng lên trên)
    float baseJumpForce = -20.0f;  // Giá trị gốc của jumpForce (để reset)
    float maxFallSpeed = 40.0f;  // Giới hạn tốc độ rơi
    float fallSpeed = 0.0f;

    PlayerState currentState = IDLE;
    bool facingRight = true;  // true = nhân vật quay phải, false = nhân vật quay trái
    bool lastFacingRight = true;  // Hướng cuối cùng trước khi dừng lại

    int frameCountIdle = 4;      // Số frame cho trạng thái đứng yên
    int frameCountRun = 4;       // Số frame cho chạy trái/phải
    int frameCountJump = 2;      // Số frame cho nhảy
    int frameCountFall = 2;      // Số frame cho rơi
    int framePunch = 4;
    int frameSword = 4;

    SDL_Rect hitbox;
    SDL_Rect hitboxWeapon;

    int map_x_;
    int map_y_;

    float hp, maxHp;
    float exp, expToLevelUp;
    int level;
    bool isLevelingUp = false;  // Kiểm tra có đang lên cấp không
    float timeLevelup;

    TTF_Font* font;

    bool isAttacking;
    float attackTimer;
    float attackDamage;
    float cooldownAttack;

    float PunchDamage;
    float SwordDamage;

    bool isPicking = false;      // Đang nhặt gỗ hay không
    float pickTimer = 0.0f;          // Bộ đếm thời gian nhặt
    const float PICK_DURATION = 2.0f; // Thời gian nhặt: 2 giây

    bool speedBoostActive = false;    // Trạng thái tăng tốc độ
    float speedBoostTimer = 0.0f;     // Thời gian còn lại của tốc độ tăng
    static const float EFFECT_DURATION;  // Thời gian hiệu ứng (30 giây)

    bool isDead = false;          // Trạng thái chết
    float deathTimer = 0.0f;      // Bộ đếm thời gian hồi sinh
    const float RESPAWN_TIME = 10.0f;  // Thời gian hồi sinh: 2 giây

    bool gameWon = false;
    bool isPaused = false;  // Trạng thái tạm dừng
    bool gameOver = false;  // Trạng thái thua game
    int deathCount = 4;     // Đếm số lần chết

    SDL_Texture* pauseTexture = NULL;
    SDL_Texture* winTexture = NULL;    // Texture khi thắng
    SDL_Texture* gameOverTexture = NULL;  // Texture khi thua
    SDL_Renderer* renderer;

    Mix_Music* backgroundMusic = nullptr;
    Game* game; // Con trỏ tới Game để gọi playJumpSound

};

#endif
