#ifndef SHOP_H
#define SHOP_H

#include <vector>
#include <string>
#include "actions.h"
#include "game.h"
#include "helpers.h"
#include "levels.h"
#include "sounds.h"



// 商店结构体
struct Shop {
    std::vector<Ritual> rituals; // 仪式列表
    std::vector<ShopItem> items; // 物品列表
    int selectedIndex; // 当前选中的物品索引
};

extern Shop shop;

void buy(); // 购买函数
void selectShopIndex(int step); // 选择商店物品索引函数
void enterShop(); // 进入商店函数
void exitShop(); // 退出商店函数
void restockShop(); // 重新进货函数
std::vector<ShopItem> createRitualItems(); // 创建仪式物品列表函数
#endif // SHOP_H
