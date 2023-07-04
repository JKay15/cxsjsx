// shop.cpp
#include "shop.h"

// 初始化商店变量
Shop shop = {
    {},
    {},
    0
};

// 购买函数实现
void buy() {
    ShopItem item = shop.items[shop.selectedIndex];
    if (item.cost <= game.souls) {
        game.souls -= item.cost;
        removeFromArray(shop.items, item);
        item.purchase();
        selectShopIndex(shop.selectedIndex);
    }
}

// 选择商店物品索引函数实现
void selectShopIndex(int step) {
    shop.selectedIndex = clamp(shop.selectedIndex + step, 0, shop.items.size() - 1);
}

// 进入商店函数实现
void enterShop() {
    game.state = SHOPPING;
    restockShop();
    game.onShopEnter();
    useShopSynths();
}

// 退出商店函数实现
void exitShop() {
    game.state = PLAYING;
    nextLevel();
    useLevelSynths();
}

// 重新进货函数实现
void restockShop() {
    int exp = pow(game.level + 1, 2);
    std::vector<ShopItem> items = {
        {10 * game.level, "Heal", "Heal 1*", [](){ actions::Damage(game.player, -1); }},
        {10 * exp, "Renew", "+1* max hp", [](){
            game.player->maxHp++;
            game.player->hp++;
        }},
        {10 * exp, "Recharge", "+1\x7F max casts", [](){ game.spell.maxCasts++; }},
        createRitualItems(),
        {0, "Continue", "Begin the next level", [](){ exitShop(); }}
    };
    shop.items = items;
}

// 创建仪式物品列表函数实现
std::vector<ShopItem> createRitualItems() {
    std::vector<Ritual> rituals = shuffled(shop.rituals.filter(ritual => game.canAddRitual(ritual)));
    std::vector<Ritual> commons = rituals.filter(r => r.rarity != RARE);
    std::vector<Ritual> rares = rituals.filter(r => r.rarity == RARE);
    std::vector<Ritual> pool = rares.slice(0, 1).concat(commons.slice(0, 2));

    std::vector<ShopItem> result;

    for (Ritual ritual : pool) {
        result.push_back({
            ritual.rarity == RARE ? 200 + randomInt(100) : 75 + randomInt(100),
            ritual.name,
            ritual.description,
            [ritual](){
                removeFromArray(shop.rituals, ritual);
                game.addRitual(ritual);
            }
        });
    }

    return result;
}
