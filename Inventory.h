
#ifndef INVENTORY_H
#define INVENTORY_H

//#include "Globals.h"
#include "TextureManager.h"

struct Item {
    std::string name;
    std::string nameid;
    int id;
    int w_weapon;
    int h_weapon;
    int quantity;  // Thêm số lượng item trong ô
    int maxStack;  // Giới hạn tối đa có thể cộng dồn (ví dụ 64)
};

// Struct cho công thức crafting
struct Recipe {
    std::vector<std::pair<Item, int>> ingredients;  // Danh sách nguyên liệu và số lượng
    Item result;  // Kết quả sau khi craft
};


class Inventory {
public:

    static const Item ITEM_EMPTY;
    static const Item ITEM_SLIME_BLOB;
    static const Item ITEM_SLIME_BOOTS;
    static const Item ITEM_SPEED_POTION;
    static Item ITEM_WOOD;  // Để có thể thay đổi quantity nếu cần
    static Item ITEM_STICK;
    static Item ITEM_GOLD;
    static Item ITEM_ROPE;
    static Item ITEM_STONE;
    static Item ITEM_STONE_SWORD;
    static Item ITEM_IRON;
    static Item ITEM_IRON_SWORD;

    Inventory();

    void addItem(const Item& item);
    Item& getHotbarItem();
    void toggleInventory();
    void openCraftingMenu();  // Mở menu crafting
    void closeCraftingMenu();  // Đóng menu crafting
    bool isCraftingMenuOpen() const;
    void toggleCraftingMenu();  // Chuyển đổi trạng thái menu
    void selectCraftingRecipe(int index);  // Chọn công thức để craft
    bool craftSelectedRecipe();  // Thực hiện craft công thức đã chọn
    bool isInventoryOpen() const;
    void selectHotbarSlot(int slot);
    void handleMouseMove(int mouseX, int mouseY);
    void handleMouseClick(int mouseX, int mouseY);
    void render(SDL_Renderer* renderer, TextureManager& textureManager);
    void clearInventory();  // Hàm xóa toàn bộ inventory

private:
    std::vector<Item> hotbar;
    std::vector<std::vector<Item>> backpack;

    bool inventoryOpen;
    bool craftingMenuOpen;  // Trạng thái menu crafting
    std::vector<Recipe> recipes;  // Danh sách công thức
    int selectedRecipeIndex;  // Công thức đang được chọn

    int hoveredHotbar = -1;
    int hoveredInventoryRow = -1;
    int hoveredInventoryCol = -1;
    int selectedHotbar = -1;
    int selectedInventoryRow = -1;
    int selectedInventoryCol = -1;

};

#endif
