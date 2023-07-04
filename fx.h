#ifndef FX_H
#define FX_H
#include "engine.h"
#include "helpers.h"
#include "game.h"
#include "sprites.h"
namespace fx{
ParticleEmitter bones(); // 骨头特效函数
ParticleEmitter trail(); // 轨迹特效函数
ParticleEmitter cloud(Rectangle area, std::vector<std::vector<Sprite>> variants); // 云特效函数
ParticleEmitter royalty(); // 皇室特效函数
ParticleEmitter dust(); // 灰尘特效函数
ParticleEmitter resurrect(GameObject& unit); // 复活特效函数
}

#endif // FX_H
