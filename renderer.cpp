// renderer.cpp
#include "renderer.h" // 包含 renderer.h 头文件


const QString ICON_SOULS = "$"; // 定义一个常量字符串 ICON_SOULS，其值为 "$"

int screenShakeTimer = 0; // 定义一个整型变量 screenShakeTimer，其初始值为 0
Point sceneOrigin(0, 150); // 定义一个 Point 类型变量 sceneOrigin，其初始值为 (0, 150)

void screenshake(QPainter &painter, double time) { // 定义 Renderer 类的 screenshake 方法，该方法接受一个 QPainter 对象和一个 double 类型变量作为参数
    screenShakeTimer = time; // 将 screenShakeTimer 的值设为 time
}

Point screenToSceneCoords(QPainter &painter, double x, double y) { // 定义 Renderer 类的 screenToSceneCoords 方法，该方法接受一个 QPainter 对象和两个 double 类型变量作为参数
    int sx = static_cast<int>(x * canvas.width()); // 计算场景坐标系中的 x 坐标
    int sy = static_cast<int>(y * canvas.height()); // 计算场景坐标系中的 y 坐标
    return { double(sx), sceneOrigin.y - sy }; // 返回计算得到的场景坐标
}

void render(QPainter &painter, double dt) { // 定义 Renderer 类的 render 方法，该方法接受一个 QPainter 对象和一个 double 类型变量作为参数
    clear(); // 调用 clear 方法清空画布
    painter.save(); // 保存 painter 的状态

    if (screenShakeTimer > 0) { // 如果 screenShakeTimer 的值大于 0
        screenShakeTimer -= dt; // 将 screenShakeTimer 的值减去 dt
        painter.translate(randomInt(2), randomInt(2)); // 随机平移 painter 的坐标系
    }

    painter.translate(sceneOrigin.x, sceneOrigin.y); // 将 painter 的坐标系平移到 sceneOrigin 的位置
    drawBackground(); // 调用 drawBackground 方法绘制背景
    drawParticles(); // 调用 drawParticles 方法绘制粒子效果
    drawObjects(painter); // 调用 drawObjects 方法绘制对象
    if (game.state == PLAYING) drawReticle(); // 如果游戏状态为 PLAYING，则调用 drawReticle 方法绘制准星
    painter.restore(); // 恢复 painter 的状态

    drawHud(painter); // 调用 drawHud 方法绘制 HUD

    if (game.state == SHOPPING) { // 如果游戏状态为 SHOPPING
        drawShop(painter); // 调用 drawShop 方法绘制商店界面
    }
}

void drawShop(QPainter &painter) { // 定义 Renderer 类的 drawShop 方法，该方法接受一个 QPainter 对象作为参数
    painter.drawText(160, 20, "Rituals\n\n"); // 在指定位置绘制文本 "Rituals\n\n"
    auto selected = shop.items[shop.selectedIndex]; // 获取当前选中的商店物品
    int y = 40; // 设置初始 y 坐标
    for (auto &item : shop.items) { // 遍历商店中的所有物品
        painter.drawText(160, y, QString("%1%2 $%3\n").arg(item.name == selected.name ? ">" : " ").arg(QString::fromStdString(item.name)).arg(item.cost));// 在指定位置绘制物品信息，包括名称和价格，如果物品被选中，则在名称前加上 ">"
        y += 20; // 更新 y 坐标以便在下一行绘制文本
    }
    painter.drawText(160, y + 20, QString::fromStdString("\n" + selected.description + "\n")); // 在指定位置绘制当前选中物品的描述

}

void drawHud(QPainter &painter) {
    if (!game.dialogue.empty()) { // 如果对话不为空
        painter.drawText(75, 50, QString::fromStdString(game.dialogue[0])); // 绘制对话文本
    }

    if (game.state == INTRO) return; // 如果游戏状态为 INTRO，则返回

    drawSprite(sprites::norman_icon, 0, 0); // 使用 drawSprite 函数绘制精灵

    for (int i = 0; i < game.player->maxHp; i++) { // 遍历玩家的最大生命值
        auto sprite = i < game.player->hp ? sprites::health_orb : sprites::health_orb_empty; // 根据玩家当前生命值选择精灵
        drawSprite(sprite, 11 + i * 4, 0); // 使用 drawSprite 函数绘制精灵
    }

    for (int i = 0; i < game.spell.maxCasts; i++) { // 遍历玩家的最大施法次数
        auto sprite = i < game.spell.casts ? sprites::cast_orb : sprites::cast_orb_empty; // 根据玩家当前施法次数选择精灵
        drawSprite(sprite, 11 + i * 4, 6); // 使用 drawSprite 函数绘制精灵
    }

    int souls = static_cast<int>(game.souls); // 获取玩家的灵魂数量
    if (souls) { // 如果玩家有灵魂
        int multiplier = game.getStreakMultiplier(); // 获取连击加成倍数
        QString bonus = multiplier ? QString("(+%1%)").arg(multiplier * 100) : ""; // 计算连击加成文本
        painter.drawText(canvas.width() / 2 - 30, 0, QString("%1%2 %3").arg(ICON_SOULS).arg(souls).arg(bonus)); // 绘制灵魂数量和连击加成文本
    }

    painter.drawText(canvas.width() - 30, 2, QString("%1-10").arg(game.level + 1)); // 绘制游戏等级文本

    if (game.state == PLAYING) { // 如果游戏状态为 PLAYING
        int x = 150;
        int y = canvas.height() - 12;
        double progress = clamp(game.ability.timer / game.ability.cooldown, 0.0, 1.0); // 计算技能冷却进度
        drawNineSlice(sprites::pink_frame, x, y, 52 * (1 - progress), 10);// 绘制技能冷却框架
        //原本的函数是drawNineSlice(sprites.pink_frame, x, y, 52 * (1 - progress) | 0, 10);
        painter.drawText(x + 10, y + 2, "Resurrect"); // 绘制技能名称文本
        if (progress == 1) painter.drawText(x + 10, y + 2, QString(" (Space)")); // 如果技能冷却完成，则绘制提示文本
        else painter.drawText(x + 10, y + 2, QString(" (%1s)").arg((1 - progress) * game.ability.cooldown / 1000)); // 否则，绘制剩余冷却时间文本
        drawSprite(sprites::skull, x + 1, y + 1); // 使用 drawSprite 函数绘制精灵
    }
}

void drawOrbs(double x, double y, double value, double maxValue, const Sprite &sprite, const Sprite &emptySprite) { // 定义 Renderer 类的 drawOrbs 方法，该方法接受一个 QPainter 对象、四个 double 类型变量和两个 QPixmap 对象作为参数
    int x0 = x - (maxValue * 4) / 2; // 计算第一个精灵图像的 x 坐标
    for (int i = 0; i < maxValue; i++) { // 遍历所有精灵图像
        drawSceneSprite(i < value ? sprite : emptySprite, x0 + i * 4, y);// 在指定位置绘制精灵图像，如果当前索引小于 value，则绘制 sprite，否则绘制 emptySprite
    }
}

void drawObjects(QPainter &painter) {
    for (auto &object : game.objects) {
        drawSceneSprite(object->sprite, object->x, object->y + object->hop); // 使用 drawSceneSprite 函数绘制精灵

        if (object->getBehaviour<Frozen>()) {
            drawNineSlice(sprites::ice, object->x, -object->sprite[3], object->sprite[2], object->sprite[3]); // 使用 drawNineSlice 函数绘制精灵
        }

        if (object->maxHp > 1 && object != game.player) {
            if (object->maxHp < 10) {
                auto x= object->center().x;
                drawOrbs(x, -6, object->hp, object->maxHp, sprites::health_orb, sprites::health_orb_empty); // 使用 drawOrbs 函数绘制精灵
            } else {
                drawSceneSprite(sprites::health_orb, object->x, -6); // 使用 drawSceneSprite 函数绘制精灵
                painter.drawText(object->x + 6, 0, QString("%1/%2").arg(object->hp).arg(object->maxHp));
            }
        }

        auto x = object->x;
        for (auto &behaviour : object->behaviours) {
            if (!behaviour->sprite.empty()) {
                drawSceneSprite(behaviour->sprite, x, -12); // 使用 drawSceneSprite 函数绘制精灵
                x += behaviour->sprite[2] + 1;
            }
        }
    }
}


void drawBackground() {
    for (int i = 0; i < game.stage.width / 16; i++) { // 遍历游戏舞台宽度
        auto sprite = i % 5 ? sprites::wall : sprites::door; // 根据位置选择墙壁或门的精灵
        drawSceneSprite(sprite, i * 16, 0);
        drawSceneSprite(sprites::floor, i * 16, -sprites::floor[3]);
        drawSceneSprite(sprites::ceiling, i * 16, game.stage.ceiling);
    }
}

void drawReticle() {
    auto [x, y] = game.getCastingPoint(); // 获取准星位置
    auto sprite = sprites::reticle; // 获取准星精灵
     drawSceneSprite(sprite, x - sprite[2] / 2, y - sprite[3] / 2); // 在准星位置绘制准星精灵
}

void drawParticles() {
    for (auto &emitter : particleEmitters) { // 遍历所有粒子发射器
        for (auto &particle : emitter->particles) { // 遍历每个粒子发射器中的所有粒子
            auto variant = emitter->variants[particle.variant]; // 获取粒子变体
            auto progress = particle.elapsed / particle.duration; // 计算粒子进度
            auto sprite = variant[static_cast<int>(progress * variant.size())]; // 获取粒子精灵
            drawSceneSprite(sprite, particle.x, particle.y); // 在粒子位置绘制粒子精灵
        }
    }
}
