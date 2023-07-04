
// fx.cpp
#include "fx.h"



// 骨头特效函数实现
ParticleEmitter bones() {
    return ParticleEmitter(
        {10000, 5000}, // 持续时间范围
        {0.6, 0}, // 摩擦力范围
        {5, 20}, // 初速度范围
        {DEG_90 - 0.5, 1}, // 角度范围
        {0.1, 0.5}, // 弹性范围
        {60, 0}, // 质量范围
        {{sprites::p_bone_1}, {sprites::p_bone_2}, {sprites::p_bone_3}} // 精灵图像变体列表
    );
}

// 轨迹特效函数实现
ParticleEmitter trail() {
    return ParticleEmitter({
        {500, 1000}, // 持续时间范围
        {0.5, 0}, // 摩擦力范围
        {1, 10}, // 初速度范围
        {M_PI, -0.5}, // 角度范围
        {0, 0}, // 弹性范围
        {3, 0}, // 质量范围
       {{sprites::p_green_1, sprites::p_green_2, sprites::p_green_3},
        {sprites::p_green_2, sprites::p_green_3, sprites::p_green_4},
        {sprites::p_green_1, sprites::p_green_2, sprites::p_green_3}},// 精灵图像变体列表
        2 // 发射频率
    });
}

// 云特效函数实现
ParticleEmitter cloud(Rectangle area, std::vector<std::vector<Sprite>> variants) {
    return ParticleEmitter({
        {500, 1000}, // 持续时间范围
        {1, 10}, // 初速度范围
        {DEG_90 - 0.2, 0.4}, // 角度范围
        {0, 0}, // 弹性范围
        variants, // 精灵图像变体列表
        2, // 发射频率
        area.x,
        area.y,
        area.w,
        area.h,
        {-2, 0} // 质量范围
    });
}

// 皇室特效函数实现
ParticleEmitter royalty() {
    return trail().extend(
        0.5, // 发射频率
        {{sprites::p_star_1, sprites::p_star_2, sprites::p_star_3},
         {sprites::p_star_2, sprites::p_star_3, sprites::p_star_4},
         {sprites::p_star_1, sprites::p_star_3}} // 精灵图像变体列表
    );
}

// 灰尘特效函数实现
ParticleEmitter dust() {
    return ParticleEmitter({
        {5000, 10000}, // 持续时间范围
        {1, 3}, // 初速度范围
        {DEG_360, DEG_360}, // 角度范围
        {0, 0}, // 弹性范围
        {{sprites::p_dust_1, sprites::p_dust_2},
         {sprites::p_dust_2, sprites::p_dust_1, sprites::p_dust_3, sprites::p_dust_1}}, // 精灵图像变体列表
        0.1, // 发射频率
        0,
        0,
        game.stage.width,
        game.stage.height,
    });
}

// 复活特效函数实现
ParticleEmitter resurrect(GameObject& unit) {
    return cloud(unit.bounds(), {{sprites::p_green_1, sprites::p_green_2, sprites::p_green_3},
                                 {sprites::p_green_2, sprites::p_green_3, sprites::p_green_4},
                                 {sprites::p_green_1, sprites::p_green_3, sprites::p_green_5}})
           .extend(0); // 发射频率
}
