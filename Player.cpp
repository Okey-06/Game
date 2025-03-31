
#include "Player.h"
#include "Game.h"

const float Player::EFFECT_DURATION = 30.0f;  // 30 giây

Player::Player(int x, int y, int speed, int p_width, int p_height)
    : x(x), y(y), speed(speed), baseSpeed(speed), velocityX(0), velocityY(0), width(p_width), height(p_height),
      frame(0), frameCount(0), frameTimer(0), map_x_(0), map_y_(0), inventory(), isPicking(false), pickTimer(0.0f) {
    srand(static_cast<unsigned>(time(0)));

    hitbox.w = width * 0.45f;
    hitbox.h = height * 0.3f;

    hitboxWeapon.x = 0;
    hitboxWeapon.y = 0;
    hitboxWeapon.w = 0;
    hitboxWeapon.h = 0;

    maxHp = 100;
    hp = maxHp;
    level = 1;
    exp = 40;
    expToLevelUp = 50;

    font = TTF_OpenFont("txt/FontPixel.ttf", 24);
    if (!font) {
        std::cerr << "❌ Không thể load font: " << TTF_GetError() << std::endl;
    }

    isAttacking = false;
    attackTimer = 0;
    PunchDamage = 10;
    SwordDamage = 30;
    frameAttack = 0;
    cooldownAttack = 2.0f;

    game = nullptr; // Khởi tạo con trỏ Game

}

void Player::useItem() {
    Item& currentItem = inventory.getHotbarItem();  // Lấy tham chiếu để sửa đổi

    if (currentItem.id == Inventory::ITEM_SPEED_POTION.id && currentItem.quantity > 0) {
        speedBoostActive = true;
        speedBoostTimer = EFFECT_DURATION;  // 30 giây
        currentItem.quantity--;  // Giảm số lượng
        if (currentItem.quantity == 0) {
            currentItem = Inventory::ITEM_EMPTY;  // Xóa item nếu hết
        }
        std::cout << "🍹 Dùng Speed Potion: Tốc độ tăng trong 30 giây!" << std::endl;
    }
    // Slime Boots không cần dùng, hiệu ứng là nội tại khi cầm
}

void Player::clean() {
    std::cout << "✅ Player cleaned!" << std::endl;
}

float Player::getX() {
    return x;
}

float Player::getY() {
    return y;
}

SDL_Rect Player::getHitbox() {
    return {hitbox.x, hitbox.y, hitbox.w, hitbox.h};
}

void Player::getTilePosition(int& row, int& col) {
    // Tính vị trí trung tâm của hitbox (điểm giữa chân nhân vật)
    float centerX = hitbox.x + hitbox.w / 2.0f;
    float centerY = hitbox.y + hitbox.h;  // Dùng chân nhân vật để xác định ô

    // Chuyển đổi từ tọa độ pixel sang tọa độ ô trên bản đồ
    col = static_cast<int>(centerX / TILE_SIZE);
    row = static_cast<int>(centerY / TILE_SIZE) - 1;

    // Giới hạn giá trị để tránh truy cập ngoài mảng mapData
    if (row < 0) row = 0;
    if (col < 0) col = 0;
}

void Player::tryPickupItem(const SDL_Event& event, GameMap& map_data) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_k) {
        if (!isPicking) {  // Chỉ bắt đầu nhặt nếu chưa nhặt
            int row, col;
            getTilePosition(row, col);
            std::vector<std::vector<int>>& mapData = map_data.getMapData();
            if (row < mapData.size() && col < mapData[row].size()) {
                if (mapData[row][col] == 2) {  // Gỗ
                    isPicking = true;  // Bắt đầu nhặt
                    pickTimer = 0.0f;  // Reset bộ đếm
                    std::cout << "Started picking wood at (" << row << ", " << col << ")" << std::endl;
                } else if (mapData[row][col] == 3) {  // Đá
                    isPicking = true;  // Bắt đầu nhặt
                    pickTimer = 0.0f;  // Reset bộ đếm
                    std::cout << "Started picking stone at (" << row << ", " << col << ")" << std::endl;
                } else {
                    std::cout << "Nothing to pick here! Tile value: " << mapData[row][col] << std::endl;
                }
            }
        }
    }
}
SDL_Rect Player::getHitboxWeapon() {
    Item weapon = inventory.getHotbarItem();
    int x_weapon = facingRight ? hitbox.x - map_x_ + hitbox.w / 2 : hitbox.x - map_x_ + hitbox.w / 2 - weapon.w_weapon;
    int y_weapon = hitbox.y - 5;

    return {x_weapon, y_weapon, weapon.w_weapon, weapon.h_weapon};
}

bool Player::checkCollisionVertical(const std::vector<std::vector<int>>& mapData, bool checkTop) {
    int leftTile = (hitbox.x) / TILE_SIZE;
    int rightTile = (hitbox.x + hitbox.w - 1) / TILE_SIZE;
    int tileY = checkTop ? (hitbox.y / TILE_SIZE) : ((hitbox.y + hitbox.h) / TILE_SIZE);

    if (tileY < 0 || tileY >= static_cast<int>(mapData.size())) return false;

    for (int col = leftTile; col <= rightTile; col++) {
        if (mapData[tileY][col] == 1) {

            return true;
        }
    }
    return false;
}

bool Player::checkCollisionHorizontal(const std::vector<std::vector<int>>& mapData) {
    int tileX = (velocityX < 0) ? (hitbox.x / TILE_SIZE) : ((hitbox.x + hitbox.w - 1) / TILE_SIZE);
    int topTile = hitbox.y / TILE_SIZE;
    int bottomTile = (hitbox.y + hitbox.h - 1) / TILE_SIZE;

    if (tileX < 0 || tileX >= static_cast<int>(mapData[0].size())) return false;

    for (int row = topTile; row <= bottomTile; row++) {
        if (mapData[row][tileX] == 1) {
            return true;
        }
    }
    return false;
}

bool Player::checkCollision(const SDL_Rect& other) {
    return SDL_HasIntersection(&hitbox, &other);
}

bool Player::checkHead(const std::vector<std::vector<int>>& mapData){
    int headTile = (int(y / TILE_SIZE));
    int leftTile = (int(hitbox.x / TILE_SIZE));
    int rightTile = (int((hitbox.x + hitbox.w - 1) / TILE_SIZE));
    if (headTile < 0 || headTile >= static_cast<int>(mapData.size())) return false;


        if (mapData[headTile][leftTile] == 1 || mapData[headTile][rightTile] == 1) {
            return true;
        }
    return false;
}

void Player::CenterPlayerOnMap(Map& map_data){

    map_data.start_x = x - SCR_W / 2;
    if(map_data.start_x < 0) map_data.start_x = 0;
    else if(map_data.start_x + SCR_W >= map_data.max_x) map_data.start_x = map_data.max_x - SCR_W;

    map_data.start_y = 0;

//    map_data.start_y = y - SCR_H / 2;
//    if(map_data.start_y < 0) map_data.start_y = 0;
//    else if(map_data.start_y + SCR_H >= map_data.max_y) map_data.start_y = map_data.max_y - SCR_H;

}

void Player::handleInput(const SDL_Event& event, GameMap& map_data,
                        std::vector<Enemy*>& enemies, std::vector<SlimeZombie*>& zombies) {
    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    velocityX = 0;

    if (keystates[SDL_SCANCODE_A]) {
        velocityX = -speed;
    }
    if (keystates[SDL_SCANCODE_D]) {
        velocityX = speed;
    }
    if (keystates[SDL_SCANCODE_SPACE] && onGround && !checkCollisionVertical(map_data.getMapData(), true) && !checkHead(map_data.getMapData())) {
        velocityY = jumpForce;
        onGround = false;
        if (game) game->playJumpSound(); // Phát âm thanh nhảy
    }

    static bool ePressed = false;
    if (keystates[SDL_SCANCODE_E]) {
        if (!ePressed) {
            inventory.toggleInventory();
            ePressed = true;
        }
    } else {
        ePressed = false;
    }

    // Chọn ô hotbar khi nhấn 1-5
    for (int i = 0; i < 5; i++) {
        if (keystates[SDL_SCANCODE_1 + i]) {
            inventory.selectHotbarSlot(i);
        }
    }


    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

    if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        inventory.handleMouseClick(mouseX, mouseY);
    } else {
        inventory.handleMouseMove(mouseX, mouseY);
    }

    if (keystates[SDL_SCANCODE_J] && !isAttacking) {
        attack(enemies, zombies);
        isAttacking = true;

    }
    tryPickupItem(event, map_data);

    // Thêm phím C để craft
    tryOpenCraftingMenu(event, map_data);

    // Xử lý chọn công thức và craft khi menu mở
    if (inventory.isCraftingMenuOpen()) {
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9) {
                int index = event.key.keysym.sym - SDLK_1;  // Chọn công thức 0, 1, 2
                inventory.selectCraftingRecipe(index);
            }
            if (event.key.keysym.sym == SDLK_RETURN) {  // Nhấn Enter để craft
                inventory.craftSelectedRecipe();
            }
        }
    }

    // Dùng item bằng phím U
    static bool uPressed = false;
    if (keystates[SDL_SCANCODE_U]) {
        if (!uPressed) {
            useItem();
            uPressed = true;
        }
    } else {
        uPressed = false;
    }

    // Nếu game đã thắng hoặc thua
    if (gameWon || gameOver) {
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_r) {  // Nhấn R để chơi lại
                inventory.clearInventory();
                resetPlayer();
                gameWon = false;
                gameOver = false;
                deathCount = 0;  // Reset số lần chết
                Mix_ResumeMusic();
                std::cout << "🔄 Game restarted!" << std::endl;
            } else if (event.key.keysym.sym == SDLK_q) {  // Nhấn Q để thoát
                Mix_HaltMusic();
                std::cout << "👋 Quitting game..." << std::endl;
                SDL_Event quitEvent;
                quitEvent.type = SDL_QUIT;
                SDL_PushEvent(&quitEvent);
            }
        }
        return;
    }

    // Xử lý tạm dừng game khi nhấn P
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p) {
        if (!isDead) {  // Chỉ cho phép tạm dừng khi không chết
            isPaused = !isPaused;  // Chuyển đổi trạng thái tạm dừng
            std::cout << (isPaused ? "⏸️ Game Paused" : "▶️ Game Resumed") << std::endl;
        }
    }

    // Nếu đang tạm dừng hoặc chết, không xử lý input khác
    if (isPaused || isDead) {
        return;
    }
}

void Player::update(float deltaTime, GameMap& map_data) {

    // Reset speed và jumpForce về giá trị gốc
    speed = baseSpeed;
    jumpForce = baseJumpForce;

    // Kiểm tra item trong hotbar cho nội tại Slime Boots
    Item currentItem = inventory.getHotbarItem();
    if (currentItem.id == Inventory::ITEM_SLIME_BOOTS.id) {
        jumpForce = baseJumpForce * 1.2f;  // Tăng 50% lực nhảy vĩnh viễn khi cầm Slime Boots
        // Không cần timer vì là nội tại
    }

    // Áp dụng hiệu ứng tạm thời của Speed Potion
    if (speedBoostActive) {
        speed = baseSpeed * 1.5f;  // Tăng 50% tốc độ
        speedBoostTimer -= deltaTime;
        if (speedBoostTimer <= 0.0f) {
            speedBoostActive = false;  // Hết hiệu ứng
            std::cout << "🏃 Hiệu ứng Speed Potion đã hết!" << std::endl;
        }
    }

    if (isPaused) {
        return;
    }

    if (hp <= 0 || y >= map_data.getMap().max_y - width) {  // max_y là chiều cao tối đa của map
        if (!isDead) {
            isDead = true;
            deathTimer = RESPAWN_TIME;  // Đặt thời gian hồi sinh
            deathCount++;  // Tăng số lần chết
            std::cout << "💀 Player has died! Respawning in " << RESPAWN_TIME << " seconds..." << std::endl;
        }
        if (deathCount >= 5) {
            gameOver = true;
            std::cout << "☠️ Game Over! You died 5 times!" << std::endl;
            Mix_HaltMusic();  // Dừng nhạc khi thua
            return;
        }
    }

    // Xử lý thời gian hồi sinh
    if (isDead) {
        deathTimer -= deltaTime;
        if (deathTimer <= 0) {
            inventory.clearInventory();  // Xóa inventory
            resetPlayer();               // Reset player
            isDead = false;              // Thoát trạng thái chết
        }
        return;  // Dừng xử lý update khi đang chết
    }

    // Kiểm tra điều kiện thắng game
    if (!gameWon && !gameOver) {
        int tileRow, tileCol;
        getTilePosition(tileRow, tileCol);
        std::vector<std::vector<int>>& mapData = map_data.getMapData();

        if (tileRow >= 0 && tileRow < mapData.size() && tileCol >= 0 && tileCol < mapData[0].size()) {
            if (mapData[tileRow][tileCol] == 5 && level >= 50) {
                gameWon = true;
                std::cout << "🎉 Congratulations! You have reached the goal and achieved level 50! You Win!" << std::endl;
                Mix_HaltMusic();  // Dừng nhạc khi thắng
                return;
            }
        }
    }
    // Nếu đã thắng, không xử lý thêm
    if (gameWon || gameOver) {
        return;
    }

    if (isPicking) {
        if (velocityX != 0) {  // Nếu di chuyển
            isPicking = false;
            pickTimer = 0.0f;
            std::cout << "Picking canceled due to movement!" << std::endl;
        }
        else {
            pickTimer += deltaTime;
            if (pickTimer >= PICK_DURATION) {
                int row, col;
                getTilePosition(row, col);
                std::vector<std::vector<int>>& mapData = map_data.getMapData();
                if (row < mapData.size() && col < mapData[row].size()) {
                    if (mapData[row][col] == 2) {  // Gỗ
                        inventory.addItem(Inventory::ITEM_WOOD);
                        std::cout << "Picked up: " << Inventory::ITEM_WOOD.name << std::endl;
                    } else if (mapData[row][col] == 3) {  // Đá
                        inventory.addItem(Inventory::ITEM_STONE);
                        std::cout << "Picked up: " << Inventory::ITEM_STONE.name << std::endl;
                    }
                }
                isPicking = false;
                pickTimer = 0.0f;
            }
        }
    }

    hp += deltaTime;
    if (hp > maxHp) hp = maxHp;


    // Tăng 1 EXP/s
    exp += deltaTime;
    if (exp >= expToLevelUp) {
        exp -= expToLevelUp;
        level++;
        expToLevelUp += 25 + level * 5;  // Mỗi level cần EXP cao hơn
        maxHp += 10;  // Tăng giới hạn máu mỗi level

        isLevelingUp = true;
        timeLevelup = 1.5;
    }

    if(isLevelingUp){
        timeLevelup -= deltaTime;
        if(timeLevelup <= 0) isLevelingUp = false;
    }

    PlayerState previousState = currentState;


    //std::cout << checkCollisionVertical(mapData, true) << std::endl; //SDL_Delay(20);

    //std::cout << checkHead(mapData) << std::endl; //SDL_Delay(16);

    if (!onGround) {
        velocityY += gravity * deltaTime * 0.75f;  // Áp dụng trọng lực theo khung hình
        if (velocityY > maxFallSpeed) {  // Giới hạn tốc độ rơi
            velocityY = maxFallSpeed;
        }
    }

    // Xử lý trạng thái nhân vật
    if (isAttacking) {
        velocityX = 0;  // Không cho di chuyển khi tấn công
        attackTime += deltaTime;
        if (attackTime >= attackTimer) {  // Khi hết thời gian animation
            isAttacking = false;
            frameAttack = 0;
            frame = 0;
            attackTime = 0.0f;
            currentState = IDLE;  // Trở về trạng thái đứng yên
        } else {
            frameAttack = static_cast<int>((attackTime / attackTimer) * frameAttackCount);  // Tính frame hiện tại
            if (frameAttack >= frameAttackCount) frameAttack = frameAttackCount - 1;  // Giới hạn frame
        }
    }
    else {
        if (velocityY < 0) {
            currentState = JUMPING;
            frameCount = frameCountJump;
        } else if (velocityY > 0 && !onGround) {
            currentState = FALLING;
            frameCount = frameCountFall;
        } else if (velocityX < 0) {
            currentState = RUNNING_LEFT;
            frameCount = frameCountRun;
            facingRight = false;
            lastFacingRight = false;
        } else if (velocityX > 0) {
            currentState = RUNNING_RIGHT;
            frameCount = frameCountRun;
            facingRight = true;
            lastFacingRight = true;
        } else {
            currentState = IDLE;
            frameCount = frameCountIdle;
        }
    }

    // Cập nhật vị trí nhân vật theo deltaTime
    x += velocityX * deltaTime;  // Điều chỉnh tốc độ di chuyển
    hitbox.x = x + (width - hitbox.w) / 2;  // Căn giữa theo chiều ngang

    // Kiểm tra va chạm ngang
    if (velocityX != 0 && checkCollisionHorizontal(map_data.getMapData())) {
        if(velocityX < 0){
            x = (int(x / TILE_SIZE) + 1) * TILE_SIZE - (width - hitbox.w) / 2 + 0;
            velocityX = 0;
        }
        if(velocityX > 0){
            velocityX = 0;
            x = (int(x / TILE_SIZE)) * TILE_SIZE + (width - hitbox.w) / 2 - 0;
        } // Căn chỉnh sát tường
    }

    y += velocityY * deltaTime * 50 ; //SDL_Delay(30);  // Điều chỉnh tốc độ rơi
    hitbox.y = y + height - hitbox.h  ; // Căn giữa theo chiều dọc

    // Kiểm tra va chạm với trần nhà khi nhảy
    if (velocityY < 0 && checkCollisionVertical(map_data.getMapData(), true)) {
        velocityY = 0;  // Dừng nhảy ngay lập tức
        hitbox.y = (int(y / TILE_SIZE) + 1) * TILE_SIZE ;// Đưa nhân vật xuống để tránh kẹt
    }

    // Kiểm tra va chạm với mặt đất
    if (velocityY > 0 && checkCollisionVertical(map_data.getMapData(), false)) {
        velocityY = 0;
        onGround = true;
        y = (int(y / TILE_SIZE)) * TILE_SIZE;
    }
    else if (velocityY == 0 && !checkCollisionVertical(map_data.getMapData(), false)) {
        onGround = false;  // Nếu không va chạm với đất nữa, đặt lại trạng thái trên không
    }

    CenterPlayerOnMap(map_data.getMap());
    // Giữ nhân vật trong màn hình
    if (hitbox.x < 0) x = - (width - hitbox.w) / 2;
    if (y < 0){
        y = 0;
        velocityY = gravity * deltaTime;
    }
    if (x + width > SCR_W + map_x_) x = map_data.getMap().max_x - width;
    //if (y + height > SCR_H) y = SCR_H - height;

    // Nếu trạng thái thay đổi, reset frame về 0
    if (currentState != previousState) {
        frame = 0;
        frameTimer = 0;
    }

    // Animation chỉ chạy khi di chuyển
    if (!isAttacking) {
        frameTimer += deltaTime;
        if (frameTimer >= 0.1f) {
            frame = (frame + 1) % frameCount;
            frameTimer -= 0.1f;
        }
    }



    //std::cout << "DeltaTime: " << deltaTime << " | FrameTimer: " << frameTimer << " | Frame: " << frame << std::endl;

    //std::cout << ", x: " << x << ", y: " << y << "Hitbox.y: " << hitbox.y << std::endl;
}


void Player::takeDamage(float dmg) {
    hp -= dmg;
    if (hp < 0) hp = 0;
}

void Player::addExp(float expAmount) {
    exp += expAmount;
}

void Player::attack(std::vector<Enemy*>& enemies, std::vector<SlimeZombie*>& zombies) {
    Item weapon = inventory.getHotbarItem();
    attackTime = 0.0f;  // Reset thời gian tấn công
    frameAttack = 0;    // Reset frame tấn công

    if (weapon.id == Inventory::ITEM_EMPTY.id) {  // Tay không
        currentState = PUNCHING;
        frameAttackCount = framePunch;  // 4 frame
        attackDamage = PunchDamage;
        attackTimer = 0.4f;  // 4 frame * 0.1s
    } else if (weapon.id == Inventory::ITEM_STONE_SWORD.id) {  // Kiếm đá
        std::cout << "⚔️ Nhân vật vung kiếm đá!" << std::endl;
        currentState = SWORD_ATTACK;
        frameAttackCount = frameSword;  // 4 frame
        attackDamage = SwordDamage;
        attackTimer = 1.2f;  // Thời gian dài hơn cho kiếm đá
    } else if (weapon.id == Inventory::ITEM_IRON_SWORD.id) {  // Kiếm sắt
        std::cout << "⚔️ Nhân vật vung kiếm sắt!" << std::endl;
        currentState = SWORD_ATTACK;
        frameAttackCount = frameSword;  // 4 frame
        attackDamage = SwordDamage * 2;
        attackTimer = 0.8f;  // Nhanh hơn kiếm đá
    } else {  // Mặc định là đấm nếu id không xác định
        currentState = PUNCHING;
        frameAttackCount = framePunch;  // 4 frame
        attackDamage = PunchDamage;
        attackTimer = 0.4f;
    }

    checkAttackCollision(enemies, zombies);
}

bool Player::checkAttackCollision(std::vector<Enemy*>& enemies, std::vector<SlimeZombie*>& zombies) {
    SDL_Rect weaponHitbox = getHitboxWeapon();
    bool hit = false;

    // Xử lý Scarecrow (Enemy)
    for (auto& enemy : enemies) {
        SDL_Rect enemyHitbox = enemy->getHitbox();
        if (SDL_HasIntersection(&weaponHitbox, &enemyHitbox)) {
            enemy->takeDamage(attackDamage);
            hit = true;
            if (enemy->getHp() <= 0) {  // Khi Scarecrow chết
                // Tỉ lệ rơi ITEM_ROPE:
                if (rand() % 100 < 30) {
                    inventory.addItem(Inventory::ITEM_ROPE);
                    std::cout << "Scarecrow dropped: " << Inventory::ITEM_ROPE.name << std::endl;
                }
                if (rand() % 100 < 50) {
                    inventory.addItem(Inventory::ITEM_STICK);
                    std::cout << "Scarecrow dropped: " << Inventory::ITEM_ROPE.name << std::endl;
                }
                // Tỉ lệ rơi ITEM_GOLD:
                if (rand() % 100 < 3) {
                    inventory.addItem(Inventory::ITEM_GOLD);
                    std::cout << "Scarecrow dropped: " << Inventory::ITEM_GOLD.name << std::endl;
                }
                addExp(SCARECROW_EXP_REWARD);  // Thêm EXP khi giết Scarecrow
            }
        }
    }

    // Xử lý SlimeZombie
    for (auto& zombie : zombies) {
        SDL_Rect zombieHitbox = zombie->getZbHitbox();
        if (SDL_HasIntersection(&weaponHitbox, &zombieHitbox)) {
            zombie->takeDamage(attackDamage);
            hit = true;
            if (zombie->getHp() <= 0) {  // Khi SlimeZombie chết
                // Tỉ lệ rơi ITEM_SLIME_BLOB:
                if (rand() % 100 < 30) {
                    inventory.addItem(Inventory::ITEM_SLIME_BLOB);
                    std::cout << "SlimeZombie dropped: " << Inventory::ITEM_SLIME_BLOB.name << std::endl;
                }

                // Tỉ lệ rơi ITEM_GOLD: 10%
                if (rand() % 100 < 4) {
                    inventory.addItem(Inventory::ITEM_GOLD);
                    std::cout << "SlimeZombie dropped: " << Inventory::ITEM_GOLD.name << std::endl;
                }
                addExp(SLIMEZOMBIE_EXP_REWARD);  // Thêm EXP khi giết SlimeZombie
            }
        }
    }
    return hit;
}


void Player::render(SDL_Renderer* renderer, TextureManager& textureManager) {

    if (!isDead && !gameWon && !gameOver && !isPaused) {
        SDL_Texture* playerTexture = textureManager.getTexture("player");
        if (!playerTexture) {
            std::cerr << "Error: Player texture not found!" << std::endl;
            return;
        }

        int texW, texH;
        SDL_QueryTexture(playerTexture, NULL, NULL, &texW, &texH);

        int frameWidth = texW / frameCountRun;
        int frameHeight = texH / frameCountState;

        int row;
        if (currentState == RUNNING_RIGHT) row = 0;
        else if (currentState == RUNNING_LEFT) row = 1;
        else if (currentState == JUMPING) row = facingRight ? 2 : 4;
        else if (currentState == FALLING) row = facingRight ? 3 : 5;
        else if (currentState == IDLE) row = lastFacingRight ? 6 : 7;


        SDL_Rect srcRect = { frame * frameWidth, row * frameHeight, frameWidth, frameHeight }; //std::cout << texW << " " << texH << std::endl;  // Cắt toàn bộ ảnh
        SDL_Rect destRect = { x - map_x_, y - map_y_, width, height };


        SDL_RenderCopy(renderer, playerTexture, &srcRect, &destRect);
       // std::cout << "Rendering player at: x=" << destRect.x << ", y=" << destRect.y
        //          << ", w=" << destRect.w << ", h=" << destRect.h << std::endl;

        if (isLevelingUp) {
            renderLevelup(renderer, textureManager);
        }

        if(isAttacking){
            renderAttack(renderer, textureManager);
        }
        SDL_Texture* resumeTexture = textureManager.getTexture("resume_icon");
        int pauseW, pauseH;
        SDL_QueryTexture(resumeTexture, NULL, NULL, &pauseW, &pauseH);

        const int RESUME_SIZE = 32;
        SDL_Rect resumeRect = { SCR_W - RESUME_SIZE - 10, 10, RESUME_SIZE, RESUME_SIZE };  // Góc phải trên
        SDL_RenderCopy(renderer, resumeTexture, NULL, &resumeRect);
    }

    // Hiển thị màn hình chết
    if (isDead) {
        SDL_Color textColor = {255, 255, 255, 255};
        std::string respawnText = "Respawning in " + std::to_string(static_cast<int>(deathTimer + 0.5f)) + "s";
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, respawnText.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        SDL_Rect textRect = { SCR_W / 2 - textSurface->w / 2, SCR_H / 2 - textSurface->h / 2, textSurface->w, textSurface->h };
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

   // Hiển thị màn hình thắng
    if (gameWon) {
        winTexture = textureManager.getTexture("gamewin");
        SDL_Rect winRect = {0, 0, SCR_W, SCR_H};  // Toàn màn hình
        SDL_RenderCopy(renderer, winTexture, NULL, &winRect);

        SDL_Color textColor = {255, 255, 0, 255};
        std::string winText = "You Win! Reached the Goal at Level " + std::to_string(level) + "! Press R to Restart or Q to Quit";
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, winText.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        SDL_Rect textRect = { SCR_W / 2 - textSurface->w / 2, 100, textSurface->w, textSurface->h };
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
    if(gameOver) {
        gameOverTexture = textureManager.getTexture("gameover");
        SDL_Rect winRect = {0, 0, SCR_W, SCR_H};  // Toàn màn hình
        SDL_RenderCopy(renderer, gameOverTexture, NULL, &winRect);

        SDL_Color textColor = {255, 255, 0, 255};
        std::string overText = "GameOver at Level " + std::to_string(level) + "! Press R to Restart or Q to Quit";
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, overText.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        SDL_Rect textRect = { SCR_W / 2 - textSurface->w / 2, 100, textSurface->w, textSurface->h };
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

    }
    // Hiển thị màn hình tạm dừng
    if (isPaused) {
        SDL_Color textColor = {255, 255, 255, 255};
        std::string pauseText = "Paused - Press P to Resume";
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, pauseText.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        SDL_Rect textRect = { SCR_W / 2 - textSurface->w / 2, SCR_H / 2 - textSurface->h / 2, textSurface->w, textSurface->h };
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        SDL_Texture* pauseTexture = textureManager.getTexture("pause_icon");
        int pauseW, pauseH;
        SDL_QueryTexture(pauseTexture, NULL, NULL, &pauseW, &pauseH);

        const int PAUSE_SIZE = 32;
        SDL_Rect pauseRect = { SCR_W - PAUSE_SIZE - 10, 10, PAUSE_SIZE, PAUSE_SIZE };  // Góc phải trên
        SDL_RenderCopy(renderer, pauseTexture, NULL, &pauseRect);
    }
    renderHealthBar(renderer, textureManager);
    renderExpBar(renderer, textureManager);
    renderLevel(renderer);
}

void Player::renderHealthBar(SDL_Renderer* renderer, TextureManager& textureManager) {

    SDL_Texture* healthTexture = textureManager.getTexture("healbar");
    SDL_Texture* grayhealthTexture = textureManager.getTexture("grayheal");

    int texW, texH;
    SDL_QueryTexture(healthTexture, NULL, NULL, &texW, &texH);

    float currenthealthRect = (hp / maxHp) * texW;
    float currentRect = (hp / maxHp) * barWidth;

    SDL_Rect scrCurrentHealthRect = { 0, 0, currenthealthRect, texH };
    SDL_Rect scrHealthLineRect = { 0, 0, texW, texH};

    SDL_Rect destHealthRect = { healbarX , healbarY, currentRect, barHeight };
    SDL_Rect destHealthLineRect = {healbarX, healbarY, barWidth, barHeight};
    SDL_RenderCopy(renderer, grayhealthTexture, &scrHealthLineRect, &destHealthLineRect);
    SDL_RenderCopy(renderer, healthTexture, &scrCurrentHealthRect, &destHealthRect);
}

void Player::renderExpBar(SDL_Renderer* renderer, TextureManager& textureManager){

    SDL_Texture* expfullTexture = textureManager.getTexture("expFull");
    SDL_Texture* expnotfullTexture = textureManager.getTexture("exp!Full");
    int texW, texH;
    SDL_QueryTexture(expfullTexture, NULL, NULL, &texW, &texH);

    float currenthealthRect = (exp / expToLevelUp) * texW;
    float currentRect = (exp / expToLevelUp) * barWidth;

    SDL_Rect scrCurrentHealthRect = { 0, 0, currenthealthRect, texH };
    SDL_Rect scrHealthLineRect = { 0, 0, texW, texH};

    SDL_Rect destHealthRect = { expX , expY, currentRect / 2, barHeight / 2 };
    SDL_Rect destHealthLineRect = {expX, expY, barWidth / 2, barHeight / 2};
    SDL_RenderCopy(renderer, expnotfullTexture, &scrHealthLineRect, &destHealthLineRect);
    SDL_RenderCopy(renderer, expfullTexture, &scrCurrentHealthRect, &destHealthRect);
}

void Player::renderLevelup(SDL_Renderer* renderer, TextureManager& textureManager){
    SDL_Texture* levelupTexture = textureManager.getTexture("levelup");

    int texW, texH;
    int frameCountLevelup = 8;
    SDL_QueryTexture(levelupTexture, NULL, NULL, &texW, &texH);

    int frameW = texW / frameCountLevelup;

    SDL_Rect scrLevelup = {frame * frameW, 0, frameW, texH};
    SDL_Rect destLevelup = {x - map_x_, y - 20, 64, 64};
    SDL_RenderCopy(renderer, levelupTexture, &scrLevelup, &destLevelup);
}

void Player::renderLevel(SDL_Renderer* renderer) {
    SDL_Color textColor = {255, 255, 255, 255};  // Màu chữ
    std::string levelText = std::to_string(level);

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, levelText.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int boxWidth = textSurface->w + 15;
    int boxHeight = textSurface->h ;
    SDL_Rect boxRect = {expX - 30, expY, boxWidth, boxHeight};

    // 🔹 Vẽ nền của level UI
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 180);
    SDL_RenderFillRect(renderer, &boxRect);

    // 🔹 Vẽ viền
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &boxRect);

    // 🔹 Vẽ số level
    SDL_Rect textRect = {boxRect.x + 10 , boxRect.y , textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void Player::renderAttack(SDL_Renderer* renderer, TextureManager& textureManager){
    Item weapon = inventory.getHotbarItem();
    SDL_Texture* weaponTexture = NULL;
    weaponTexture =  textureManager.getTexture("attack");
    int texW, texH;
    SDL_QueryTexture(weaponTexture, NULL, NULL, &texW, &texH);
    int frameW = texW / frameAttackCount;
    int row = lastFacingRight ? 0 : 1;
    int x_weapon = facingRight ? hitbox.x - map_x_ + hitbox.w / 2 : hitbox.x - map_x_ + hitbox.w / 2
     - frameW;
    int y_weapon = hitbox.y + (hitbox.h - weapon.h_weapon) / 2 ;
    SDL_Rect scrWeapon = {frameAttack * frameW, row * texH / 2, frameW, texH / 2};
    SDL_Rect destWeapon = {x_weapon, y_weapon, weapon.w_weapon, weapon.h_weapon};
    SDL_RenderCopy(renderer, weaponTexture, &scrWeapon, &destWeapon);
//    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
//    SDL_RenderDrawRect(renderer, &destWeapon);
}

void Player::tryOpenCraftingMenu(const SDL_Event& event, GameMap& map_data) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_c) {
        int row, col;
        getTilePosition(row, col);
        std::vector<std::vector<int>>& mapData = map_data.getMapData();
        if (row < mapData.size() && col < mapData[row].size() && mapData[row][col] == 4) {
            inventory.toggleCraftingMenu();
        } else {
            std::cout << "❌ Phải đứng trên ô crafting (giá trị 4) để mở menu!" << std::endl;
            inventory.closeCraftingMenu();  // Đóng menu nếu không đứng trên ô 4
        }
    }
}

void Player::resetPlayer() {
    // Reset vị trí về điểm xuất phát (ví dụ: góc trái trên của map)
    x = PlayerSpawnX;
    y = PlayerSpawnY;
    hitbox.x = x + (width - hitbox.w) / 2;
    hitbox.y = y + height - hitbox.h;

    // Reset vận tốc
    velocityX = 0;
    velocityY = 0;

    // Reset trạng thái
    currentState = IDLE;
    isAttacking = false;
    attackTime = 0.0f;
    frameAttack = 0;
    onGround = false;  // Sẽ được cập nhật lại trong update nếu cần

    // Reset HP
    maxHp = MAXHP;
    hp = maxHp;

    // Reset EXP và level (tùy bạn muốn giữ level hay không)
    exp = 0;
    level = 1;
    expToLevelUp = 50;

    // Reset các hiệu ứng
    speedBoostActive = false;
    speedBoostTimer = 0.0f;
    isPicking = false;
    pickTimer = 0.0f;

    // Reset animation
    frame = 0;
    frameTimer = 0.0f;
    facingRight = true;
    lastFacingRight = true;

    deathCount = 0;  // Reset số lần chết

    std::cout << "✅ Player has been reset!" << std::endl;
}
