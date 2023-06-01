#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <string>
//#include "Sprite.h"
//#include "ParticleEmitter.h"

typedef std::vector<int> Sprite;

struct Rect
{
    int x;
    int y;
    int w;
    int h;
};

// 其他结构体和类型定义

void clear();
void drawSprite(const Sprite &sprite, int x, int y);
void drawSceneSprite(const Sprite &sprite, int x, int y);
void drawSpriteSlice(int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh);
void drawNineSlice(const Sprite &sprite, int x, int y, int w, int h);
void write(const std::string &text, int x = textX, int y = textY);

class ParticleEmitter
{
public:
    // 成员变量定义

    ParticleEmitter();

    // 成员函数定义
};

// 其他类定义

#endif // ENGINE_H
