// behaviours.cpp
#include "behaviours.h"
#include "fx.h"
#include "sprites.json.h"

std::map<int, SpellCounter> HitStreak::counters;

// 攻击行为类的碰撞处理函数实现
void Attack::onCollision(GameObject& target) {
    // 计算造成伤害和承受伤害
    int dealDamage = this->object.hp;
    int takeDamage = target.hp;
    // 对目标造成伤害
    Damage(target, dealDamage, &this->object);
    // 对自身承受伤害
    Damage(this->object, takeDamage, &target);
}

// 消失计时器行为类的构造函数实现
DespawnTimer::DespawnTimer(GameObject& object, int duration) : Behaviour(object), duration(duration) {}

// 消失计时器行为类的帧更新函数实现
void DespawnTimer::onFrame(int dt) {
    // 更新已经过去的时间
    if ((this->elapsed += dt) >= this->duration) {
        // 如果已经过去的时间大于等于持续时间，则将对象从游戏中移除
        game.despawn(this->object);
    }
}

// 行军行为类的构造函数实现
March::March(GameObject& object, int step) : Behaviour(object), step(step) {}

// 行军行为类的更新函数实现
void March::onUpdate() {
    // 如果对象正在飞行，则返回
    if (this->object.y > 0) return;

    // 使用补间动画使对象移动并跳跃
    tween(this->object.x, this->object.x + this->step, 200, [this](int x, float t){
        this->object.x = x;
        this->object.hop = sin(t * M_PI) * 2;
        if (t == 1 && this->object.mass >= 100) screenshake(50);
    });

    // 如果对象走出屏幕，则将其从游戏中移除
    if (
        (this->step < 0 && this->object.x < 0) ||
        (this->step > 0 && this->object.x > game.stage.width)
    ) {
        game.despawn(this->object);
    }
}

// 造成伤害行为类的碰撞处理函数实现
void Damaging::onCollision(GameObject& target) {
    // 对目标造成伤害
    Damage(target, this->amount, &this->object);
}

// 流血行为类的更新函数实现
void Bleeding::onUpdate() {
    // 更新特效发射器位置并发射特效
    this->emitter.extend(this->object.center());
    this->emitter.burst(1);
    // 对对象造成伤害
    Damage(this->object, 1, &this->object);
}

// 愤怒行为类的构造函数实现
Enraged::Enraged(GameObject& object, int mask) : Behaviour(object), mask(mask) {}

// 愤怒行为类的受到伤害处理函数实现
void Enraged::onDamage(Damage& damage) {
    // 如果伤害来源对象存在且具有指定掩码，则阻止伤害并播放特效
    if (damage.dealer && damage.dealer->is(this->mask)) {
        Damage(this->object, -damage.amount, &this->object);
        damage.amount = 0;
        this->emitter.extend(this->object.bounds()).burst(4);
    }
}

// 寻找行为类的帧更新函数实现
void Seeking::onFrame() {
    auto& projectile = this->object;
    GameObject* target = nullptr;
    float minDist = 100;

    // 遍历游戏中的对象列表，寻找最近的具有指定掩码的对象
    for (auto& object : game.objects) {
        if (object.is(this->object.collisionMask)) {
            float dist = distance(projectile, object);
            if (dist < minDist) {
                target = &object;
                minDist = dist;
            }
        }
    }

    // 如果找到目标，则调整弹道朝向目标
    if (target) {
        float currentAngle = vectorToAngle(projectile.vx, projectile.vy);
        float desiredAngle = angleBetweenPoints(projectile, target->center());
        float angle = currentAngle + (desiredAngle - currentAngle) / 20;
        float magnitude = hypot(projectile.vx, projectile.vy);
        auto [vx, vy] = vectorFromAngle(angle);
        projectile.vx = vx * magnitude;
        projectile.vy = vy * magnitude;
    }
}

// 召唤行为类的构造函数实现
Summon::Summon(GameObject& object, std::function<GameObject()> create, int summonSpeed)
: Behaviour(object), create(create), summonSpeed(summonSpeed) {}

// 召唤行为类的召唤处理函数实现（空函数）
void Summon::onSummon(GameObject& object) {}

// 召唤行为类的帧更新函数实现
void Summon::onFrame(int dt) {
    // 更新召唤计时器
    if ((this->summonTimer += dt) > this->summonSpeed) {
        // 如果召唤计时器大于召唤速度，则重置计时器并增加召唤计数器
        this->summonTimer = 0;
        this->summonCounter++;
        // 创建对象并将其加入游戏
        auto object = this->create();
        game.spawn(object, this->object.x, this->object.y);
        // 调用召唤处理函数
        this->onSummon(object);
    }
}
// 连击行为类的碰撞处理函数实现
void HitStreak::onCollision(GameObject& target) {
    // 设置命中标记为真
    this->hit = true;
}

// 连击行为类的添加处理函数实现
void HitStreak::onAdded() {
    // 获取或创建计数器
    this->counter = HitStreak::counters[this->object.groupId];
    if (!this->counter) {
        this->counter = {0, 0};
        HitStreak::counters[this->object.groupId] = this->counter;
    }
    // 增加计数器总数
    this->counter.total++;
}

// 连击行为类的移除处理函数实现
void HitStreak::onRemoved() {
    // 如果命中标记为真，则增加计数器命中数
    if (this->hit) this->counter.hits++;
    // 减少计数器总数
    if (--this->counter.total) return;

    // 如果计数器命中数大于 0，则增加游戏连击值，否则重置游戏连击值
    if (this->counter.hits) {
        game.streak = clamp(game.streak + 1, 0, MAX_STREAK);
    } else {
        game.streak = 0;
    }
}

// 无敌行为类的受到伤害处理函数实现
void Invulnerable::onDamage(Damage& damage) {
    // 如果伤害量大于 0，则阻止伤害
    if (damage.amount > 0) damage.amount = 0;
}

// 冰冻行为类的更新函数实现
bool Frozen::onUpdate() {
    // 更新冰冻计时器
    if (this->freezeTimer-- <= 0) {
        // 如果冰冻计时器小于等于 0，则移除该行为
        this->object.removeBehaviour(this);
    }

    return true;
}

// 闪电打击行为类的碰撞处理函数实现
void LightningStrike::onCollision(GameObject& target) {
    int bolts = 3;
    for (int i = 0; i < bolts; i++) {
        auto bolt = LightningSpell();
        bolt.vy = -200;
        bolt.vx = randomInt(20) - 10;
        bolt.y = clamp(50 + randomInt(100), 0, game.stage.ceiling - 10);
        bolt.x = target.x + randomInt(50) - 25;
        game.spawn(bolt);
    }
}
