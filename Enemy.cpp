#include "Enemy.h"
#include "Player.h"

Enemy::Enemy(float x, float y, float maxhealth, Player* player) : x(x), y(y), isAlive(true), respawnTimer(0), p(player), expReward(0) {

    hitbox = { x , y, 64, 64 }; // Giả định kích thước quái

    hp = maxhealth;
    maxHp = maxhealth;

    frameTimer = 0;
    frame = 0;
    frameCount = 0;
}

void Enemy::update(float deltaTime, Player& player) {
    if (player.GetisPaused()) {
        return;
    }
    if (!isAlive) {
        respawnTimer -= deltaTime;
        if (respawnTimer <= 0) {
            hp = 100;  // Reset HP
            isAlive = true;
        }
    }
    frameTimer += deltaTime;
    if (frameTimer >= 0.1f) {
        frame = (frame + 1) % frameScarecrowCount;
        frameTimer -= 0.1f;
    }

}

void Enemy::render(SDL_Renderer* renderer, TextureManager& textureManager) {
    if (isAlive) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &hitbox); // Hiển thị hitbox tạm thời
    }
}

void Enemy::takeDamage(float damage) {
    if (isAlive) {
        hp -= damage;
        if (hp <= 0) {
            isAlive = false;
            respawnTimer = 5.0f; // Hồi sinh sau 5 giây
            if (p) {  // Check if p is not null
                p->addExp(expReward);
                std::cout << "Scarecrow awarded " << expReward << " EXP to player" << std::endl;
            } else {
                std::cerr << "Error: Player pointer is null in Scarecrow::takeDamage" << std::endl;
            }

        }
    }
}

bool Enemy::checkCollision(SDL_Rect weaponHitbox) {
    return SDL_HasIntersection(&hitbox, &weaponHitbox);
}

// ================== Scarecrow ==================
Scarecrow::Scarecrow(float x, float y, Player* player) : Enemy(x, y, 100, player) {
    expReward = SCARECROW_EXP_REWARD;
}

void Scarecrow::render(SDL_Renderer* renderer, TextureManager& textureManager)  {
    if (isAlive) {
        SDL_Texture* scarecrowTexture = textureManager.getTexture("scarecrow");
        int texW, texH;
        SDL_QueryTexture(scarecrowTexture, NULL, NULL, &texW, &texH);
        int frameW = texW / frameScarecrowCount;
        SDL_Rect scrRect = {frame * frameW, 0, frameW, texH};
        SDL_Rect renderRect = { hitbox.x - map_x_, hitbox.y, hitbox.w, hitbox.h };
        SDL_RenderCopy(renderer, scarecrowTexture, &scrRect, &renderRect);

        SDL_Rect hpBar = {hitbox.x - map_x_, hitbox.y - 10, hitbox.w, 5};  // Thanh HP nền (đen)
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(renderer, &hpBar);

        int hpWidth = (hitbox.w * hp) / maxHp; // Tính phần trăm HP
        SDL_Rect currentHp = {hitbox.x - map_x_, hitbox.y - 10, hpWidth, 5};
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &currentHp);



//        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Màu đỏ
//        SDL_Rect hitboxHide = {hitbox.x - x, hitbox.y, hitbox.w, hitbox.h};
//        SDL_RenderFillRect(renderer, &hitboxHide);
    }
}

SlimeZombie::SlimeZombie(float spawnX, float spawnY, float zb_start, float zb_end, Player* p) : zbTexture(NULL), currentState(IDLE) {
    this->p = p;
    x = spawnX;
    y = spawnY;
    zb_hitbox = { static_cast<int>(x), static_cast<int>(y), width, height };  // Khởi tạo hitbox
    startX = zb_start;
    endX = zb_end;
    lastRight = true;
    speed = 20;
    maxHp = 600;
    hp = maxHp;
    damage = 100;
    attackCooldown = 1.0f;
    attackCooldownLeft = attackCooldown;
    frame = 0;
    frameCount = 1;
    velocityX = 0;
    velocityY = 0;
    frameTimer = 0;
    attackTime = 0;
    dyingTime = 0;
    isAlive = true;
    expReward = SLIMEZOMBIE_EXP_REWARD;
}

bool SlimeZombie::checkCollisionVertical(const std::vector<std::vector<int>>& mapData){
    int leftTile = x / TILE_SIZE;
    int rightTile = (x + width - 1) / TILE_SIZE;
    int tileY =(y + height) / TILE_SIZE;

    if (tileY < 0 || tileY >= static_cast<int>(mapData.size())) return false;

    for (int col = leftTile; col <= rightTile; col++) {
        if (mapData[tileY][col] == 1) {
            return true;
        }
    }
    return false;
}

void SlimeZombie::update(float deltaTime, GameMap& map_data, Player& player) {

    velocityX = direction * speed;

    if (player.GetisPaused()) {
        return;
    }

    State previousState = currentState;

    // Handle DEAD state first to avoid unnecessary processing
    if (currentState == DEAD) {
        velocityX = 0;
        respawnTimer -= deltaTime;
        if (respawnTimer <= 0) {
            hp = maxHp;
            currentState = IDLE;
            hasDamage = false;
            frame = 0;  // Reset frame for IDLE state
            frameCount = frameCountIdle;  // Set correct frame count
        }
        return;
    }

    // Reset animation variables if state changes
    if (currentState != previousState) {
        frame = 0;
        frameTimer = 0;
        attackTime = 0;
        dyingTime = 0;
        hasDamage = false;
    }

    // Gravity and falling logic
    if (!onGround) {
        velocityY += ENEMY_GRAVITY * deltaTime;
        if (velocityY > ENEMY_FALLMAX) {
            velocityY = ENEMY_FALLMAX;  // Cap falling speed
        }
    }

    // Attack cooldown
    if (attackCooldownLeft > 0) {
        attackCooldownLeft -= deltaTime;
    }

    // Attack initiation logic
    if (currentState != ATTACKINGLEFT && currentState != ATTACKINGRIGHT &&
        currentState != DYING && currentState != DEAD && attackCooldownLeft <= 0) {
        float distance = std::abs(zb_hitbox.x + zb_hitbox.w / 2 - playerX);
        if (playerY > zb_hitbox.y && playerY < (zb_hitbox.y + zb_hitbox.h) && distance < zb_hitbox.w / 2 + 32) {
            if (playerX < zb_hitbox.x + zb_hitbox.w / 2) {
                currentState = ATTACKINGLEFT;
                lastRight = false;
            } else {
                currentState = ATTACKINGRIGHT;
                lastRight = true;
            }
            frameCount = frameCountAttack;  // Set correct frame count for attack animation
            frame = 0;
            hasDamage = false;
        }
    }

    // State-specific logic
    if (currentState != ATTACKINGLEFT && currentState != ATTACKINGRIGHT &&
        currentState != DYING && currentState != DEAD) {
        if (velocityY > 0 && !onGround) {
            currentState = IDLE;
            frameCount = frameCountIdle;
        } else if (velocityX < 0) {
            currentState = MOVINGLEFT;
            frameCount = frameCountMove;
            lastRight = false;
        } else if (velocityX > 0) {
            currentState = MOVINGRIGHT;
            frameCount = frameCountMove;
            lastRight = true;
        } else {
            currentState = IDLE;
            frameCount = frameCountIdle;
        }
    } else {
        velocityX = 0;  // Stop movement during special states
        if (currentState == DYING || currentState == DEAD) {
            frameCount = frameCountDead;  // Set frame count for dying animation
        } else if (currentState == ATTACKINGLEFT || currentState == ATTACKINGRIGHT) {
            frameCount = frameCountAttack;  // Ensure frame count is set for attack
        }
    }

    // Position updates
    x += velocityX * deltaTime * 2.0f;
    zb_hitbox.x = x + (width - zb_hitbox.w) / 2;

    // Boundary checking
    if (x < startX) {
        direction = 1;
        x = startX;
    }
    if (x > endX) {
        direction = -1;
        x = endX;
    }

    y += velocityY * deltaTime;
    zb_hitbox.y = y;

    // Collision with ground
    if (velocityY > 0 && checkCollisionVertical(map_data.getMapData())) {
        velocityY = 0;
        onGround = true;
        y = (int(y / TILE_SIZE)) * TILE_SIZE + 15;
    }

    // Handle attacking animation
    if (currentState == ATTACKINGLEFT || currentState == ATTACKINGRIGHT) {
        attackTime += deltaTime;
        if (attackTime >= (attackTimer / frameCountAttack)) {
            attackTime = 0;
            frame++;
        }
        if (p && frame == frameCountAttack / 2 && !hasDamage) {  // Damage at midpoint of animation
            SDL_Rect playerHitbox = p->getHitbox();
            if (SDL_HasIntersection(&zb_hitbox, &playerHitbox)) {
                p->takeDamage(damage);
                hasDamage = true;
            }
        }
        if (frame >= frameCountAttack) {  // End attack animation
            attackCooldownLeft = attackCooldown;
            currentState = IDLE;
            frame = 0;
            frameCount = frameCountIdle;
            hasDamage = false;
        }
    }
    // Handle dying animation
    else if (currentState == DYING) {
        dyingTime += deltaTime;
        if (dyingTime >= (deathTimer / frameCountDead)) {
            dyingTime = 0;
            frame++;
            if (frame >= frameCountDead) {
                currentState = DEAD;
                frame = 0;  // Reset frame for DEAD state
            }
        }
    }
    // Regular animation for moving/idle states
    else {
        frameTimer += deltaTime;
        if (frameTimer >= 0.1f) {
            frame = (frame + 1) % frameCount;
            frameTimer -= 0.1f;
        }
    }
}

void SlimeZombie::takeDamage(float dmg) {
        if (currentState != DYING && currentState != DEAD){
            hp -= dmg;
            std::cout << "SlimeZombie HP: " << hp << std::endl;
            if (hp <= 0) {
                std::cout << "SlimeZombie HP: " << hp << std::endl;
                currentState = DYING;
                deathTimer = 3;
                respawnTimer = 20;
                if (p) {  // Check if p is not null
                    p->addExp(expReward);
                    std::cout << "Scarecrow awarded " << expReward << " EXP to player" << std::endl;
                } else {
                    std::cerr << "Error: Player pointer is null in Scarecrow::takeDamage" << std::endl;
                }
            }
    }
}
void SlimeZombie::render(SDL_Renderer* renderer, TextureManager& textureManager) {
    zbTexture = textureManager.getTexture("slimezombie");
    if (!zbTexture) {
        std::cerr << "Error: zombie texture not found!" << std::endl;
        return;
    }

    if(currentState != DEAD){
        int row;
        if (currentState == MOVINGLEFT) row = 2;
        else if (currentState == MOVINGRIGHT) row = 3;
        else if (currentState == ATTACKINGLEFT) row = 4;
        else if (currentState == ATTACKINGRIGHT) row = 5;
        else if (currentState == IDLE) row = lastRight ? 1 : 0;
        else if (currentState == DYING) row = 6;


        SDL_Rect srcRect = { frame * width, row * height, width, height };
        SDL_Rect destRect = { x - map_x_, y, width, height };
        SDL_RenderCopy(renderer, zbTexture, &srcRect, &destRect);

        SDL_Rect hpBar = {x - map_x_, y - 10, width, 5};  // Thanh HP nền (đen)
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(renderer, &hpBar);

        int hpWidth = (width * hp) / maxHp; // Tính phần trăm HP
        SDL_Rect currentHp = {x - map_x_, y - 10, hpWidth, 5};
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &currentHp);

//        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Màu đỏ
//        SDL_Rect hitboxHide = {zb_hitbox.x - map_x_, zb_hitbox.y, zb_hitbox.w, zb_hitbox.h};
//        SDL_RenderDrawRect(renderer, &hitboxHide);
    }
}

void SlimeZombie::GetTexture(TextureManager& textureManager) {
    if (zbTexture) {
        SDL_DestroyTexture(zbTexture);
    }
    zbTexture = textureManager.getTexture("slimezombie");
    if (!zbTexture) {
        std::cerr << "Error: Failed to load SlimeZombie texture!" << std::endl;
        return;
    }
    int texW, texH;
    SDL_QueryTexture(zbTexture, NULL, NULL, &texW, &texH);
    width = texW / frameCountAttack;
    height = texH / frameState;
    zb_hitbox.w = width * 0.6;
    zb_hitbox.h = height;
}
