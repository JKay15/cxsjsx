// actions.cpp
#include "actions.h"

// 造成伤害函数实现
void Damage(GameObject* object, int amount, GameObject* dealer) {
    // 创建伤害对象
    struct Damage damage = { amount, dealer };
    // 调用对象的 onDamage 函数
    object->onDamage(damage);
    // 更新对象的生命值
    object->hp = clamp(object->hp - damage.amount, 0, object->maxHp);
    // 如果对象生命值为 0，则调用死亡函数
    if (!object->hp) actions::Die(object, dealer);
}

// 死亡函数实现
void Die(GameObject* object, GameObject* killer) {
    // 创建死亡对象
    Death death = {
        object,
        killer,
        object->souls
    };

    // 如果对象具有 MOBILE 标签
    if (object->is(MOBILE)) {
        // 获取对象中心坐标
        auto center = object->center();

        // 播放骨头特效
        fx::bones()
            .extend(center.x,center.y)
            .burst(2 + randomInt(3))
            .remove();

        // 遍历游戏中的仪式列表，调用仪式的 onDeath 函数（如果存在）
        for (auto* ritual : game.rituals) {
            if (ritual->onDeath) ritual->onDeath(death);
        }

        // 如果随机数小于等于对象的尸体概率，则生成尸体
        if (randomFloat() <= object->corpseChance) {
            game.spawn(Corpse(), center.x, center.y);
        }

        // 增加游戏灵魂值
        game.addSouls(death.souls);
    }

    // 调用对象的 onDeath 函数
    object->onDeath(death);

    // 将对象从游戏中移除
    game.despawn(object);
}

std::chrono::milliseconds castAnimationTimeout{0};
int castGroupId = 1;

// 施法函数实现
void Cast() {
    auto& spell = game.spell;
    auto& player = game.player;

    // 如果法术施放次数为 0，则返回
    if (spell.casts == 0) return;
    spell.casts--;

    // 更新玩家精灵为举起双臂状态
    player->sprite = sprites::norman_arms_up;
    castAnimationTimeout = std::chrono::milliseconds(500);
    std::this_thread::sleep_for(castAnimationTimeout);
    player->sprite = sprites::norman_arms_down;

    int power = spell.basePower;
    float targetAngle = spell.targetAngle - (spell.shotsPerRound * spell.shotOffsetAngle / 2);
    int groupId = castGroupId++;

    // 遍历每一发子弹
    for (int j = 0; j < spell.shotsPerRound; j++) {
        auto projectile = Spell();
        float angle = targetAngle + j * spell.shotOffsetAngle;
        auto res = vectorFromAngle(angle);
        double vx = res[0];
        double vy = res[1];
        auto [x, y] = game.getCastingPoint();
        projectile.x = x - projectile.sprite[2] / 2;
        projectile.y = y - projectile.sprite[3] / 2;
        projectile.vx = vx * power;
        projectile.vy = vy * power;
        projectile.groupId = groupId;
        game.spawn(projectile);
        game.onCast(&projectile);
    }
}

// 复活函数实现
void Resurrect() {
    // 如果能力计时器小于能力冷却时间，则返回
    if (game.ability.timer < game.ability.cooldown) {
        return;
    }

    // 重置能力计时器
    game.ability.timer = 0;

    // 遍历游戏中的仪式列表，调用仪式的 onResurrect 函数（如果存在）
    for (auto& ritual : game.rituals) {
        if (ritual->onResurrect) ritual->onResurrect();
    }

    // 获取游戏中所有尸体对象
    std::vector<GameObject> corpses;
    std::copy_if(game.objects.begin(), game.objects.end(), std::back_inserter(corpses),
        [](const GameObject& object) { return object.is(CORPSE); });

    // 遍历尸体对象列表
    for (auto& corpse : corpses) {
        // 将尸体从游戏中移除
        game.despawn(&corpse);

        // 创建骷髅单位并将其加入游戏
        auto unit = Skeleton();
        game.spawn(unit, corpse.x, 0);
        fx::resurrect(unit).burst(10).remove();

        // 遍历游戏中的仪式列表，调用仪式的 onResurrection 函数（如果存在）
        for (auto& ritual : game.rituals) {
            if (ritual->onResurrection) ritual->onResurrection(&unit);
        }
    }
}
