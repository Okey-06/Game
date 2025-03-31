#include "Inventory.h"

const Item Inventory::ITEM_EMPTY = {"Tay không", "hand", 0, 64, 32, 1, 1};
const Item Inventory::ITEM_SLIME_BLOB = {"Cục slime", "slime_blob", 1, 0, 0, 1, 64};
const Item Inventory::ITEM_SLIME_BOOTS = {"Giày slime", "slime_boots", 2, 0, 0, 1, 1};
const Item Inventory::ITEM_SPEED_POTION = {"Thuốc tốc độ", "speed_potion", 3, 0, 0, 1, 64};
Item Inventory::ITEM_WOOD = {"Gỗ", "wood", 4, 0, 0, 1, 64};  // Không const để thay đổi quantity nếu cần
Item Inventory::ITEM_STICK = {"Gậy", "stick", 5, 32, 16, 1, 64};
Item Inventory::ITEM_GOLD = {"Vàng", "gold", 6, 48, 16, 1, 64};
Item Inventory::ITEM_ROPE = {"Dây buộc", "rope", 7, 0, 0, 1, 64};
Item Inventory::ITEM_STONE = {"Đá", "stone", 8, 0, 0, 1, 64};
Item Inventory::ITEM_STONE_SWORD = {"Kiếm đá", "stone_sword", 9, 48, 16, 1, 1};
Item Inventory::ITEM_IRON = {"Sắt", "iron", 10, 0, 0, 1, 64};
Item Inventory::ITEM_IRON_SWORD = {"Kiếm sắt", "iron_sword", 11, 64, 16, 1, 1};

Inventory::Inventory() : inventoryOpen(false), craftingMenuOpen(false), selectedHotbar(0), selectedInventoryRow(-1),
                        selectedInventoryCol(-1), selectedRecipeIndex(-1) {
    hotbar.resize(hotbarSize, Inventory::ITEM_EMPTY);
    backpack.resize(invRows, std::vector<Item>(invCols, Inventory::ITEM_EMPTY));

    // Định nghĩa công thức crafting theo yêu cầu của bạn
    recipes = {
        // 16 Wood + 32 Rope + 32 Stone + 8 Gold -> 1 Stone Sword
        { {{ITEM_WOOD, 16}, {ITEM_ROPE, 32}, {ITEM_STONE, 32}, {ITEM_GOLD, 8}}, ITEM_STONE_SWORD },

        // 16 Wood + 32 Rope + 16 Iron + 16 Gold -> 1 Iron Sword
        { {{ITEM_WOOD, 16}, {ITEM_ROPE, 32}, {ITEM_IRON, 16}, {ITEM_GOLD, 16}}, ITEM_IRON_SWORD },

        // 4 Stick -> 1 Wood
        { {{ITEM_STICK, 4}}, ITEM_WOOD },

        // 8 Wood -> 1 Gold
        { {{ITEM_WOOD, 8}}, ITEM_GOLD },

        // 16 Rope -> 1 Gold
        { {{ITEM_ROPE, 16}}, ITEM_GOLD },

        // 4 Stone + 1 Gold -> 1 Iron
        { {{ITEM_STONE, 4}, {ITEM_GOLD, 1}}, ITEM_IRON },

        // 1 Iron -> 1 Gold
        { {{ITEM_IRON, 1}}, ITEM_GOLD },

        // 16 Slime Blob -> 1 Slime Boots
        { {{ITEM_SLIME_BLOB, 16}}, ITEM_SLIME_BOOTS },

        // 32 Gold -> 1 Speed Potion
        { {{ITEM_GOLD, 32}}, ITEM_SPEED_POTION }
    };
}

void Inventory::addItem(const Item& item) {
    int remainingQuantity = item.quantity;  // Số lượng cần thêm

    // Ưu tiên cộng dồn vào các ô đã có item cùng loại trong hotbar
    for (int i = 0; i < hotbarSize && remainingQuantity > 0; i++) {
        if (hotbar[i].id == item.id && hotbar[i].quantity < hotbar[i].maxStack) {
            int spaceLeft = hotbar[i].maxStack - hotbar[i].quantity;
            int amountToAdd = std::min(remainingQuantity, spaceLeft);
            hotbar[i].quantity += amountToAdd;
            remainingQuantity -= amountToAdd;
            std::cout << "📦 Cộng dồn " << amountToAdd << " " << item.name << " vào hotbar slot " << i << std::endl;
        }
    }

    // Nếu vẫn còn số lượng cần thêm, cộng dồn vào backpack
    for (int row = 0; row < invRows && remainingQuantity > 0; row++) {
        for (int col = 0; col < invCols && remainingQuantity > 0; col++) {
            if (backpack[row][col].id == item.id && backpack[row][col].quantity < backpack[row][col].maxStack) {
                int spaceLeft = backpack[row][col].maxStack - backpack[row][col].quantity;
                int amountToAdd = std::min(remainingQuantity, spaceLeft);
                backpack[row][col].quantity += amountToAdd;
                remainingQuantity -= amountToAdd;
                std::cout << "📦 Cộng dồn " << amountToAdd << " " << item.name << " vào backpack (" << row << ", " << col << ")" << std::endl;
            }
        }
    }
    // Nếu vẫn còn số lượng, tìm ô trống để thêm
    if (remainingQuantity > 0) {
        // Thêm vào hotbar trước
        for (int i = 0; i < hotbarSize && remainingQuantity > 0; i++) {
            if (hotbar[i].id == 0) {  // Ô trống
                hotbar[i] = item;
                hotbar[i].quantity = std::min(remainingQuantity, item.maxStack);
                remainingQuantity -= hotbar[i].quantity;
                std::cout << "📦 Thêm " << hotbar[i].quantity << " " << item.name << " vào hotbar slot " << i << std::endl;
                if (remainingQuantity <= 0) return;
            }
        }

        // Thêm vào backpack nếu hotbar đầy
        for (int row = 0; row < invRows && remainingQuantity > 0; row++) {
            for (int col = 0; col < invCols && remainingQuantity > 0; col++) {
                if (backpack[row][col].id == 0) {  // Ô trống
                    backpack[row][col] = item;
                    backpack[row][col].quantity = std::min(remainingQuantity, item.maxStack);
                    remainingQuantity -= backpack[row][col].quantity;
                    std::cout << "📦 Thêm " << backpack[row][col].quantity << " " << item.name << " vào backpack (" << row << ", " << col << ")" << std::endl;
                    if (remainingQuantity <= 0) return;
                }
            }
        }
    }

    if (remainingQuantity > 0) {
        std::cout << "❌ Không thể thêm " << remainingQuantity << " " << item.name << ", inventory đã đầy!" << std::endl;
    }
}

// Mở menu crafting
void Inventory::openCraftingMenu() {
    craftingMenuOpen = true;
    selectedRecipeIndex = -1;  // Reset lựa chọn
    std::cout << "📋 Mở menu crafting" << std::endl;
}

// Đóng menu crafting
void Inventory::closeCraftingMenu() {
    craftingMenuOpen = false;
    selectedRecipeIndex = -1;
    std::cout << "❌ Đóng menu crafting" << std::endl;
}

// Chuyển đổi trạng thái menu
void Inventory::toggleCraftingMenu() {
    if (craftingMenuOpen) {
        closeCraftingMenu();
    } else {
        openCraftingMenu();
    }
}

// Kiểm tra menu có mở không
bool Inventory::isCraftingMenuOpen() const {
    return craftingMenuOpen;
}

// Chọn công thức
void Inventory::selectCraftingRecipe(int index) {
    if (index >= 0 && index < recipes.size()) {
        selectedRecipeIndex = index;
        std::cout << "📌 Chọn công thức: " << recipes[index].result.name << std::endl;
    }
}

// Craft công thức đã chọn
bool Inventory::craftSelectedRecipe() {
    if (selectedRecipeIndex < 0 || selectedRecipeIndex >= recipes.size()) {
        std::cout << "❌ Chưa chọn công thức nào!" << std::endl;
        return false;
    }

    Recipe& recipe = recipes[selectedRecipeIndex];

    // Kiểm tra đủ nguyên liệu
    std::map<int, int> required;  // id -> số lượng cần
    for (const auto& [item, qty] : recipe.ingredients) {
        required[item.id] += qty;
    }

    std::map<int, int> available;  // id -> số lượng có
    for (const auto& item : hotbar) {
        if (item.id != 0) available[item.id] += item.quantity;
    }
    for (const auto& row : backpack) {
        for (const auto& item : row) {
            if (item.id != 0) available[item.id] += item.quantity;
        }
    }

    for (const auto& [id, qty] : required) {
        if (available[id] < qty) {
            std::cout << "❌ Không đủ nguyên liệu để craft " << recipe.result.name << "!" << std::endl;
            return false;
        }
    }

    // Xóa nguyên liệu
    for (const auto& [ingredient, qty] : recipe.ingredients) {
        int remaining = qty;
        for (auto& slot : hotbar) {
            if (slot.id == ingredient.id && remaining > 0) {
                int amountToRemove = std::min(remaining, slot.quantity);
                slot.quantity -= amountToRemove;
                remaining -= amountToRemove;
                if (slot.quantity == 0) slot = ITEM_EMPTY;
            }
        }
        for (auto& row : backpack) {
            for (auto& slot : row) {
                if (slot.id == ingredient.id && remaining > 0) {
                    int amountToRemove = std::min(remaining, slot.quantity);
                    slot.quantity -= amountToRemove;
                    remaining -= amountToRemove;
                    if (slot.quantity == 0) slot = ITEM_EMPTY;
                }
            }
        }
    }

    // Thêm kết quả
    addItem(recipe.result);
    std::cout << "✅ Crafted: " << recipe.result.name << std::endl;
    return true;
}

void Inventory::selectHotbarSlot(int slot) {
    if (slot >= 0 && slot < hotbarSize) {
        selectedHotbar = slot;
        std::cout << "📌 Đã chọn ô " << slot + 1 << ": " << hotbar[slot].name << std::endl;
    }
}

Item& Inventory::getHotbarItem() {
    return hotbar[selectedHotbar];
}

void Inventory::toggleInventory() {
    inventoryOpen = !inventoryOpen;
    std::cout << (inventoryOpen ? "📦 Mở inventory" : "❌ Đóng inventory") << std::endl;
}

bool Inventory::isInventoryOpen() const {
    return inventoryOpen;
}

void Inventory::render(SDL_Renderer* renderer, TextureManager& textureManager) {
    TTF_Font* font = TTF_OpenFont("txt/FontPixel.ttf", 32);  // Font cho số lượng
    if (!font) {
        std::cerr << "❌ Không thể load font: " << TTF_GetError() << std::endl;
        return;
    }

    int totalWidth = invCols * (slotSize + spacing) - spacing;
    int startX = (SCR_W - totalWidth) / 2;
    int startYHotbar = SCR_H - 70;
    int startYInventory = startYHotbar - (invRows * (slotSize + spacing)) - 20;

    // Vẽ hotbar
    for (int i = 0; i < hotbarSize; i++) {
        SDL_Rect slot = { startX + i * (slotSize + spacing), startYHotbar, slotSize, slotSize };
        if (i == selectedHotbar) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Viền đỏ khi chọn
        } else if (i == hoveredHotbar) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Viền xanh khi hover
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        }
        SDL_RenderDrawRect(renderer, &slot);

        SDL_Texture* texture = textureManager.getTexture(hotbar[i].nameid);
        if (texture) {
            SDL_RenderCopy(renderer, texture, nullptr, &slot);
        }

        // Vẽ số lượng nếu > 1
        if (hotbar[i].quantity > 1) {
            SDL_Color white = {255, 255, 255, 255};
            std::string qtyText = std::to_string(hotbar[i].quantity);
            SDL_Surface* surface = TTF_RenderText_Solid(font, qtyText.c_str(), white);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect textRect = {slot.x + slotSize - 20, slot.y + slotSize - 20, surface->w, surface->h};
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(textTexture);
        }
    }

    // Vẽ menu crafting
    if (craftingMenuOpen) {
        int menuWidth = 400;  // Tăng chiều rộng để chứa texture
        int menuHeight = recipes.size() * 60 + 20;  // Tăng chiều cao mỗi dòng
        int menuX = (SCR_W - menuWidth) / 2;
        int menuY = (SCR_H - menuHeight) / 2;

        // Vẽ nền menu
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 200);
        SDL_Rect menuRect = {menuX, menuY, menuWidth, menuHeight};
        SDL_RenderFillRect(renderer, &menuRect);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &menuRect);

        // Vẽ danh sách công thức
        for (int i = 0; i < recipes.size(); i++) {
            SDL_Rect recipeRect = {menuX + 10, menuY + 10 + i * 60, menuWidth - 20, 50};
            if (i == selectedRecipeIndex) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Viền đỏ khi chọn
                SDL_RenderDrawRect(renderer, &recipeRect);
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            }

            int xOffset = 0;

            // Vẽ nguyên liệu
            for (const auto& [item, qty] : recipes[i].ingredients) {
                SDL_Rect itemRect = {recipeRect.x + xOffset, recipeRect.y + 5, 40, 40};
                SDL_Texture* texture = textureManager.getTexture(item.nameid);
                if (texture) {
                    SDL_RenderCopy(renderer, texture, nullptr, &itemRect);
                }

                // Vẽ số lượng nguyên liệu
                SDL_Color white = {255, 255, 255, 255};
                std::string qtyText = std::to_string(qty);
                SDL_Surface* surface = TTF_RenderText_Solid(font, qtyText.c_str(), white);
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_Rect textRect = {itemRect.x + 30, itemRect.y + 25, surface->w, surface->h};
                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                SDL_FreeSurface(surface);
                SDL_DestroyTexture(textTexture);

                xOffset += 50;  // Dịch sang phải cho nguyên liệu tiếp theo
            }

            // Vẽ dấu "->"
            SDL_Color white = {255, 255, 255, 255};
            SDL_Surface* arrowSurface = TTF_RenderText_Solid(font, "->", white);
            SDL_Texture* arrowTexture = SDL_CreateTextureFromSurface(renderer, arrowSurface);
            SDL_Rect arrowRect = {recipeRect.x + xOffset + 10, recipeRect.y + 15, arrowSurface->w, arrowSurface->h};
            SDL_RenderCopy(renderer, arrowTexture, nullptr, &arrowRect);
            SDL_FreeSurface(arrowSurface);
            SDL_DestroyTexture(arrowTexture);
            xOffset += 40;

            // Vẽ kết quả
            SDL_Rect resultRect = {recipeRect.x + xOffset, recipeRect.y + 5, 40, 40};
            SDL_Texture* resultTexture = textureManager.getTexture(recipes[i].result.nameid);
            if (resultTexture) {
                SDL_RenderCopy(renderer, resultTexture, nullptr, &resultRect);
            }

            // Vẽ số "1" cho kết quả (luôn là 1)
            SDL_Surface* resultQtySurface = TTF_RenderText_Solid(font, "1", white);
            SDL_Texture* resultQtyTexture = SDL_CreateTextureFromSurface(renderer, resultQtySurface);
            SDL_Rect resultQtyRect = {resultRect.x + 30, resultRect.y + 25, resultQtySurface->w, resultQtySurface->h};
            SDL_RenderCopy(renderer, resultQtyTexture, nullptr, &resultQtyRect);
            SDL_FreeSurface(resultQtySurface);
            SDL_DestroyTexture(resultQtyTexture);

            // Vẽ số thứ tự công thức (1, 2, 3, ...)
            std::string indexText = std::to_string(i + 1) + ".";
            SDL_Surface* indexSurface = TTF_RenderText_Solid(font, indexText.c_str(), white);
            SDL_Texture* indexTexture = SDL_CreateTextureFromSurface(renderer, indexSurface);
            SDL_Rect indexRect = {recipeRect.x - 20, recipeRect.y + 15, indexSurface->w, indexSurface->h};
            SDL_RenderCopy(renderer, indexTexture, nullptr, &indexRect);
            SDL_FreeSurface(indexSurface);
            SDL_DestroyTexture(indexTexture);
        }
    }
    TTF_CloseFont(font);
}

void Inventory::handleMouseMove(int mouseX, int mouseY) {
    int totalWidth = invCols * (slotSize + spacing) - spacing;
    int startX = (SCR_W - totalWidth) / 2;
    int startYHotbar = SCR_H - 70;
    int startYInventory = startYHotbar - (invRows * (slotSize + spacing)) - 20;

    hoveredHotbar = -1;
    hoveredInventoryRow = -1;
    hoveredInventoryCol = -1;

    // 🔹 Kiểm tra hover vào hotbar
    for (int i = 0; i < hotbarSize; i++) {
        SDL_Rect slot = { startX + i * (slotSize + spacing), startYHotbar, slotSize, slotSize };
        if (mouseX >= slot.x && mouseX <= slot.x + slotSize &&
            mouseY >= slot.y && mouseY <= slot.y + slotSize) {
            hoveredHotbar = i;
            return;
        }
    }

    // 🔹 Kiểm tra hover vào inventory (nếu mở)
    if (inventoryOpen) {
        for (int row = 0; row < invRows; row++) {
            for (int col = 0; col < invCols; col++) {
                SDL_Rect slot = { startX + col * (slotSize + spacing), startYInventory + row * (slotSize + spacing), slotSize, slotSize };
                if (mouseX >= slot.x && mouseX <= slot.x + slotSize &&
                    mouseY >= slot.y && mouseY <= slot.y + slotSize) {
                    hoveredInventoryRow = row;
                    hoveredInventoryCol = col;
                    return;
                }
            }
        }
    }
}

void Inventory::handleMouseClick(int mouseX, int mouseY) {
    int totalWidth = hotbarSize * (slotSize + spacing) - spacing;
    int startX = (SCR_W - totalWidth) / 2;
    int startY = SCR_H - 70 - 120;

    if (hoveredHotbar != -1) {
        if (selectedHotbar == -1 && selectedInventoryRow == -1) {
            selectedHotbar = hoveredHotbar;
        } else if (selectedHotbar != -1) {
            std::swap(hotbar[selectedHotbar], hotbar[hoveredHotbar]);
            selectedHotbar = -1;
        } else if (selectedInventoryRow != -1) {
            std::swap(backpack[selectedInventoryRow][selectedInventoryCol], hotbar[hoveredHotbar]);
            selectedInventoryRow = -1;
            selectedInventoryCol = -1;
        }
        return;
    }

    if (hoveredInventoryRow != -1) {
        if (selectedHotbar == -1 && selectedInventoryRow == -1) {
            selectedInventoryRow = hoveredInventoryRow;
            selectedInventoryCol = hoveredInventoryCol;
        } else if (selectedInventoryRow != -1) {
            std::swap(backpack[selectedInventoryRow][selectedInventoryCol], backpack[hoveredInventoryRow][hoveredInventoryCol]);
            selectedInventoryRow = -1;
            selectedInventoryCol = -1;
        } else if (selectedHotbar != -1) {
            std::swap(hotbar[selectedHotbar], backpack[hoveredInventoryRow][hoveredInventoryCol]);
            selectedHotbar = -1;
        }
    }
}

void Inventory::clearInventory() {
    // Xóa toàn bộ hotbar
    for (int i = 0; i < 5; i++) {
        hotbar[i] = ITEM_EMPTY;  // Đặt lại thành ITEM_EMPTY
        hotbar[i].quantity = 0;  // Đảm bảo số lượng về 0
    }

    // Nếu có inventory chính (items), xóa luôn
    /*
    for (int i = 0; i < 20; i++) {
        items[i] = ITEM_EMPTY;
        items[i].quantity = 0;
    }
    */

    selectedHotbar = 0;  // Đặt lại slot đang chọn về 0
    std::cout << "🗑️ Inventory has been cleared!" << std::endl;
}
