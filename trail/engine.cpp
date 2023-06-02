#include "engine.h"

void clear()
{
    // 清除画布
}

void drawSprite(const Sprite &sprite, int x, int y)
{
    // 绘制精灵图像
}

void drawSceneSprite(const Sprite &sprite, int x, int y)
{
    // 绘制场景精灵图像
}

void drawSpriteSlice(int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh)
{
    // 绘制精灵图像的一部分
}

void drawNineSlice(const Sprite &sprite, int x, int y, int w, int h)
{
    // 绘制九宫格精灵图像
}

void write(const std::string &text, int x = textX, int y = textY)
{
    // 绘制文本
}

ParticleEmitter::ParticleEmitter()
{
    // 初始化成员变量
}

// 其他成员函数的实现
