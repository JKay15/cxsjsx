// behaviours.h
#ifndef BEHAVIOURS_H
#define BEHAVIOURS_H

#include <string>
#include "game.h"
#include "helpers.h"
#include "objects.h"
#include "tags.h"

// 攻击行为类
class Attack : public Behaviour {
public:
    void onCollision(GameObject& target) override; // 碰撞处理函数
};

// 消失计时器行为类
class DespawnTimer : public Behaviour {
public:
    int elapsed = 0; // 已经过去的时间
    int duration; // 持续时间

    DespawnTimer(GameObject& object, int duration); // 构造函数
    void onFrame(int dt) override; // 帧更新函数
};

// 行军行为类
class March : public Behaviour {
public:
    int step; // 步长

    March(GameObject& object, int step); // 构造函数
    void onUpdate() override; // 更新函数
};

// 造成伤害行为类
class Damaging : public Behaviour {
public:
    int amount = 1; // 伤害量
    void onCollision(GameObject& target) override; // 碰撞处理函数
};

// 流血行为类
class Bleeding : public Behaviour {
public:
    std::string sprite = sprites::status_bleeding; // 精灵图像
    int turns = 3; // 持续回合数
    int amount = 1; // 伤害量

    Emitter emitter = fx::cloud({0, 0, 0, 0}, {{sprites::health_orb, sprites::health_pip}, {sprites::health_pip}})
        .extend({10, 30, 10, 30, 0}); // 特效发射器

    void onUpdate() override; // 更新函数
};

// 愤怒行为类
class Enraged : public Behaviour {
public:
    std::string sprite = sprites::status_enraged; // 精灵图像

    Emitter emitter = fx::cloud({0, 0, 0, 0}, {{sprites::health_orb, sprites::health_pip}, {sprites::health_pip}})
        .extend({10, 30, 10, 30, 0}); // 特效发射器

    int mask; // 掩码

    Enraged(GameObject& object, int mask); // 构造函数
    void onDamage(Damage& damage) override; // 受到伤害处理函数
};

// 寻找行为类
class Seeking : public Behaviour {
public:
    void onFrame() override; // 帧更新函数
};

// 召唤行为类
class Summon : public Behaviour {
private:
    int summonTimer = 0; // 召唤计时器
public:
    int summonCounter = 0; // 召唤计数器

    std::function<GameObject()> create; // 创建对象函数
    int summonSpeed; // 召唤速度

    Summon(GameObject& object, std::function<GameObject()> create, int summonSpeed); // 构造函数
    void onSummon(GameObject& object); // 召唤处理函数
    void onFrame(int dt) override; // 帧更新函数
};

// 结构体：法术计数器
struct SpellCounter {
    int total; // 总数
    int hits; // 命中数
};

// 连击行为类
class HitStreak : public Behaviour {
public:
    static std::map<int, SpellCounter> counters; // 计数器映射表
    bool hit = false; // 是否命中标记
    SpellCounter counter; // 计数器

    void onCollision(GameObject& target) override; // 碰撞处理函数
    void onAdded() override; // 添加处理函数
    void onRemoved() override; // 移除处理函数
};

// 无敌行为类
class Invulnerable : public Behaviour {
public:
    std::string sprite = sprites::status_shielded; // 精灵图像

    void onDamage(Damage& damage) override; // 受到伤害处理函数
};

// 冰冻行为类
class Frozen : public Behaviour {
public:
    int freezeTimer = 10; // 冰冻计时器

    bool onUpdate() override; // 更新函数
};

// 闪电打击行为类
class LightningStrike : public Behaviour {
public:
    void onCollision(GameObject& target) override; // 碰撞处理函数
};

#endif
