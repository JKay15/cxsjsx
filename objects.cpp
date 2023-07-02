// objects.cpp
#include "objects.h"
#include "sprites.json.h"
#include "fx.h"
#include "sounds.h"

// 尸体对象函数实现
GameObject Corpse() {
    GameObject unit; // 创建游戏对象
    unit.sprite = sprites::skull; // 设置精灵图像
    unit.mass = 100; // 设置质量
    unit.tags = CORPSE; // 设置标签
    return unit; // 返回游戏对象
}

// 玩家对象函数实现
GameObject Player() {
    GameObject player; // 创建游戏对象
    player.x = 5; // 设置初始位置
    player.tags = PLAYER | UNDEAD; // 设置标签
    player.sprite = sprites::norman_arms_down; // 设置精灵图像
    player.collisionMask = LIVING; // 设置碰撞掩码
    player.updateSpeed = 1000; // 设置更新速度
    player.hp = player.maxHp = 5; // 设置生命值和最大生命值
    player.emitter = fx::resurrect(player); // 设置特效发射器
    player.onCollision = [&](auto& unit) { // 设置碰撞处理函数
        Damage(player, unit.hp); // 对玩家造成伤害
        Die(unit); // 杀死目标单位
    };
    player.onDeath = []() { window.location = window.location; }; // 设置死亡处理函数（重新加载页面）
    return player; // 返回游戏对象
}

// 法术对象函数实现
GameObject Spell() {
    GameObject object; // 创建游戏对象
    object.sprite = sprites::p_green_skull; // 设置精灵图像
    object.tags = SPELL; // 设置标签
    object.collisionMask = MOBILE | LIVING; // 设置碰撞掩码
    object.mass = 100; // 设置质量
    object.emitter = fx::trail(); // 设置特效发射器
    object.friction = 0.1; // 设置摩擦力
    object.despawnOnCollision = true; // 碰撞后消失标记设置为真
    object.despawnOnBounce = true; // 弹跳后消失标记设置为真
    object.addBehaviour(new Damaging(object)); // 添加造成伤害行为
    return object; // 返回游戏对象
}

// 流血法术对象函数实现
GameObject BleedSpell() {
    GameObject spell = Spell(); // 创建法术对象
    spell.sprite = sprites::p_red_skull; // 设置精灵图像
    spell.emitter->extend({ // 扩展特效发射器属性
        {sprites::p_red_3, sprites::p_red_2, sprites::p_red_1},
        {sprites::p_red_4, sprites::p_red_3, sprites::p_red_2},
        {sprites::p_red_3, sprites::p_red_2, sprites::p_red_1}
    });
    spell.addBehaviour().onCollision = [&](auto& target) { // 添加碰撞处理函数
        target.addBehaviour(new Bleeding(target)); // 给目标添加流血行为
    };
    return spell; // 返回游戏对象
}

// 闪电法术对象函数实现
GameObject LightningSpell() {
    GameObject spell = Spell(); // 创建法术对象
    spell.sprite = sprites::p_skull_yellow; // 设置精灵图像
    spell.emitter->frequency = 0.8; // 设置特效发射器发射频率
    spell.emitter->variants = { // 设置特效发射器精灵图像变体列表
        {sprites::p_lightning_1, sprites::p_lightning_2, sprites::p_lightning_3, sprites::p_lightning_4},
        {sprites::p_lightning_1, sprites::p_lightning_2, sprites::p_lightning_3, sprites::p_lightning_5},
        {sprites::p_lightning_2, sprites::p_lightning_3, sprites::p_lightning_6},
        {sprites::p_lightning_4, sprites::p_lightning_5, sprites::p_lightning_6},
        {sprites::p_purple_5}
    };
    return spell; // 返回游戏对象
}
// 骷髅士兵对象函数实现
GameObject Skeleton() {
    GameObject unit; // 创建游戏对象
    unit.sprite = sprites::skeleton; // 设置精灵图像
    unit.tags = UNDEAD | MOBILE; // 设置标签
    unit.collisionMask = LIVING; // 设置碰撞掩码
    unit.hp = unit.maxHp = 1; // 设置生命值和最大生命值
    unit.updateSpeed = 1000; // 设置更新速度
    unit.behaviours.push(new March(unit, 16)); // 添加行军行为
    return unit; // 返回游戏对象
}

// 骷髅领主对象函数实现
GameObject SkeletonLord() {
    GameObject unit = Skeleton(); // 创建骷髅士兵对象
    unit.sprite = sprites::big_skeleton; // 设置精灵图像
    unit.hp = unit.maxHp = 3; // 设置生命值和最大生命值
    unit.updateSpeed = 1500; // 设置更新速度
    return unit; // 返回游戏对象
}

// 村民对象函数实现
GameObject Villager() {
    GameObject unit; // 创建游戏对象
    unit.sprite = randomElement({sprites::villager_1, sprites::villager_2, sprites::villager_3, sprites::villager_4}); // 随机设置精灵图像
    unit.friction = 0.8; // 设置摩擦力
    unit.mass = 75; // 设置质量
    unit.x = game.stage.width; // 设置初始位置
    unit.tags = LIVING | MOBILE; // 设置标签
    unit.hp = unit.maxHp = 1; // 设置生命值和最大生命值
    unit.updateSpeed = 600; // 设置更新速度
    unit.addBehaviour(new March(unit, -16)); // 添加行军行为
    unit.corpseChance = 0.75; // 设置尸体生成概率
    unit.souls = 5; // 设置灵魂数量
    return unit; // 返回游戏对象
}

// 强盗对象函数实现
GameObject Bandit() {
    GameObject unit = Villager(); // 创建村民对象
    unit.hp = unit.maxHp = 2; // 设置生命值和最大生命值
    return unit; // 返回游戏对象
}

// 国王对象函数实现
GameObject TheKing() {
    GameObject unit = Villager(); // 创建村民对象
    unit.sprite = sprites::the_king; // 设置精灵图像
    unit.updateSpeed = 5000; // 设置更新速度
    unit.hp = unit.maxHp = 100; // 设置生命值和最大生命值
    unit.behaviours.clear(); // 清空行为列表
    unit.mass = 1000; // 设置质量

    int phase = 1; // 初始化阶段为 1

    March* marching = new March(unit, -32); // 创建行军行为对象（移动速度为 -32）
    Summon* summons = new Summon(unit, RoyalGuard, 2000); // 创建召唤行为对象（每 2 秒召唤一个王室卫士）
    Enraged* enraged = new Enraged(unit, SPELL); // 创建愤怒行为对象（对法术免疫）
    Invulnerable* invulnerable = new Invulnerable(unit); // 创建无敌行为对象
    Behaviour* boss = new Behaviour(unit); // 创建自定义行为对象

    unit.addBehaviour(marching); // 添加行军行为对象
    unit.addBehaviour(boss); // 添加自定义行为对象

    boss->onDamage = [&](Damage dmg) { // 设置自定义行为的受伤处理函数
        bool willDie = unit.hp - dmg.amount <= 0; // 计算是否会死亡

        if (phase == 1 && willDie) { // 如果处于第一阶段且会死亡，则进入第二阶段
            phase = 2; // 设置阶段为 2
            unit.addBehaviour(summons); // 添加召唤行为对象
            unit.addBehaviour(enraged); // 添加愤怒行为对象
            unit.addBehaviour(invulnerable); // 添加无敌行为对象
            marching->step *= -1; // 反转移动速度
        } else if (phase == 3 && willDie) { // 如果处于第三阶段且会死亡，则进入第四阶段
            phase = 4; // 设置阶段为 4
            unit.hp = unit.maxHp; // 恢复生命值
            unit.sprite = sprites::the_king_on_foot; // 更改精灵图像
            unit.updateSpeed = unit.updateClock = 1000; // 更改更新速度

            int t = 0;
            Behaviour* customBehaviour = new Behaviour(unit);
            customBehaviour->onFrame = [&](int dt) { // 设置自定义行为的帧处理函数
                if ((t += dt) > 300) { // 每隔 300 毫秒执行一次操作
                    t = 0;
                    game.spawn(Corpse(), randomInt(game.stage.width), game.stage.ceiling); // 在游戏中生成尸体对象
                }
            };
            unit.addBehaviour(customBehaviour);
        }
    };

    summons->onSummon = [&]() { // 设置召唤行为的召唤处理函数
        if (summons->summonCounter >= 5) { // 如果召唤次数大于等于 5，则进入第三阶段
            phase = 3; // 设置阶段为 3
            unit.removeBehaviour(enraged); // 移除愤怒行为对象
            unit.removeBehaviour(invulnerable); // 移除无敌行为对象
            unit.removeBehaviour(summons); // 移除召唤行为对象
            marching->step *= -1; // 反转移动速度
        }
    };

    return unit; // 返回游戏对象
}



// 冠军对象函数实现
GameObject Champion() {
    GameObject unit = Villager(); // 创建村民对象
    unit.sprite = sprites::champion; // 设置精灵图像
    unit.updateSpeed = 1000; // 设置更新速度
    unit.hp = unit.maxHp = 10; // 设置生命值和最大生命值
    unit.souls = 25; // 设置灵魂数量
    return unit; // 返回游戏对象
}

// 壳骑士对象函数实现
GameObject ShellKnight() {
    GameObject unit = Villager(); // 创建村民对象
    unit.sprite = sprites::shell_knight_up; // 设置精灵图像
    unit.updateSpeed = 1000; // 设置更新速度
    unit.hp = unit.maxHp = 5; // 设置生命值和最大生命值
    unit.souls = 15; // 设置灵魂数量

    Behaviour* shell = unit.addBehaviour(); // 添加自定义行为对象
    bool shelled = false; // 初始化护盾状态为假
    int timer = 0;

    shell->onUpdate = [&]() { // 设置自定义行为的更新处理函数
        shelled = timer++ % 4 > 1; // 更新护盾状态（每隔一定时间切换护盾状态）
        unit.sprite = shelled ? sprites::shell_knight_down : sprites::shell_knight_up; // 根据护盾状态更改精灵图像
        shell->sprite = shelled ? sprites::status_shielded : undefined; // 根据护盾状态更改行为精灵图像（用于显示护盾状态）
    };

    shell->onDamage = [&](auto& dmg) { // 设置自定义行为的受到伤害处理函数
        if (shelled) { // 如果处于护盾状态，则阻止伤害
            dmg.amount = std::min(0, dmg.amount);
        }
    };

    return unit; // 返回游戏对象
}

// 和尚对象函数实现
GameObject Monk() {
    GameObject unit = Villager(); // 创建村民对象
    unit.sprite = sprites::monk; // 设置精灵图像
    unit.updateSpeed = 600; // 设置更新速度
    unit.hp = unit.maxHp = 3; // 设置生命值和最大生命值
    unit.souls = 10; // 设置灵魂数量

    Behaviour* heal = new Behaviour(unit); // 创建自定义行为对象
    heal->turns = 5; // 设置自定义行为更新间隔

    heal->onUpdate = [&]() { // 设置自定义行为的更新处理函数
        for (auto& object : game.objects) { // 遍历游戏中所有对象
            if (object.is(LIVING)) { // 如果对象是活着的，则对其造成负伤害（治愈）
                Damage(object, -1, unit);
            }
        }

        fx::cloud(unit.bounds(), { // 在单位周围生成云特效
            {sprites::p_star_1, sprites::p_star_2, sprites::p_star_3},
            {sprites::p_star_2, sprites::p_star_3, sprites::p_star_4},
            {sprites::p_star_1, sprites::p_star_3}
        }).burst(10).remove(); // 爆发生成 10 个粒子并移除特效发射器
    };

    unit.addBehaviour(heal); // 添加自定义行为
    return unit; // 返回游戏对象
}

// 弓箭手对象函数实现
GameObject Archer() {
    GameObject unit = Villager(); // 创建村民对象
    unit.sprite = sprites::archer; // 设置精灵图像
    unit.updateSpeed = 300; // 设置更新速度
    unit.hp = unit.maxHp = 2; // 设置生命值和最大生命值
    return unit; // 返回游戏对象
}

// 吹笛人对象函数实现
GameObject Piper() {
    GameObject unit = Villager(); // 创建村民对象
    unit.sprite = sprites::piper; // 设置精灵图像
    unit.updateSpeed = 500; // 设置更新速度
    unit.hp = unit.maxHp = 15; // 设置生命值和最大生命值
    unit.addBehaviour(new Summon(unit, Rat, 2000)); // 添加召唤行为（召唤老鼠）
    unit.souls = 100; // 设置灵魂数量
    return unit; // 返回游戏对象
}

// 老鼠对象函数实现
GameObject Rat() {
    GameObject unit = Villager(); // 创建村民对象
    unit.sprite = sprites::rat; // 设置精灵图像
    unit.updateSpeed = 200; // 设置更新速度
    unit.souls = 5; // 设置灵魂数量
    unit.corpseChance = 0; // 设置尸体生成概率为 0
    return unit; // 返回游戏对象
}

// 愤怒骑士对象函数实现
GameObject RageKnight() {
    GameObject unit = Villager(); // 创建村民对象
    unit.sprite = sprites::rage_knight; // 设置精灵图像
    unit.updateSpeed = 500; // 设置更新速度
    unit.hp = unit.maxHp = 5; // 设置生命值和最大生命值

    Behaviour* raging = unit.addBehaviour(); // 添加自定义行为对象
    March* march = dynamic_cast<March*>(unit.getBehaviour(March)); // 获取行军行为对象指针
    Enraged* enraged = new Enraged(unit, SPELL); // 创建愤怒行为对象（对法术免疫）
    bool angry = false; // 初始化愤怒状态为假

    raging->turns = 5; // 设置自定义行为更新间隔

    raging->onUpdate = [&]() { // 设置自定义行为的更新处理函数
        angry = !angry; // 切换愤怒状态

        if (angry) { // 如果处于愤怒状态，则添加愤怒行为并更改精灵图像和移动速度
            unit.addBehaviour(enraged);
            unit.sprite = sprites::rage_knight_enraged;
            march->step = 0;
        } else { // 如果不处于愤怒状态，则移除愤怒行为并更改精灵图像和移动速度
            unit.removeBehaviour(enraged);
            unit.sprite = sprites::rage_knight;
            march->step = -16;
        }
    };

    unit.souls = 20; // 设置灵魂数量

    return unit; // 返回游戏对象
}


// 王室卫士对象函数实现
GameObject RoyalGuard() {
    GameObject unit = Villager(); // 创建村民对象
    unit.sprite = sprites::royal_guard; // 设置精灵图像
    unit.hp = unit.maxHp = 4; // 设置生命值和最大生命值
    unit.souls = 10; // 设置灵魂数量

    March* march = dynamic_cast<March*>(unit.getBehaviour(March)); // 获取行军行为对象指针

    bool shielded = false; // 初始化盾牌状态为假
    Behaviour* shield = unit.addBehaviour(); // 添加自定义行为对象
    shield->turns = 3; // 设置自定义行为更新间隔

    shield->onUpdate = [&]() { // 设置自定义行为的更新处理函数
        shielded = !shielded; // 切换盾牌状态
        march->step = shielded ? 0 : -16; // 根据盾牌状态设置移动速度
        unit.sprite = shielded ? sprites::royal_guard_shielded : sprites::royal_guard; // 根据盾牌状态设置精灵图像
    };

    shield->onDamage = [&](Damage dmg) { // 设置自定义行为的受伤处理函数
        if (!shielded || !dmg.dealer->is(SPELL)) return; // 如果没有盾牌或伤害来源不是法术，则直接返回

        if (dmg.dealer->vx > 0) { // 如果伤害来源的水平速度大于 0，则反弹法术并生成王室卫士宝珠对象
            dmg.amount = 0; // 设置伤害量为 0

            GameObject orb = RoyalGuardOrb(); // 创建王室卫士宝珠对象
            orb.vx = dmg.dealer->vx *= -1; // 设置水平速度
            orb.vy = dmg.dealer->vy *= -0.25; // 设置垂直速度
            orb.mass = dmg.dealer->mass; // 设置质量
            game.spawn(orb, dmg.dealer->x - orb.sprite[2] - 1, dmg.dealer->y); // 在游戏中生成王室卫士宝珠对象
        }
    };

    unit.behaviours.reverse(); // 反转行为列表顺序，使盾牌行为优先执行

    return unit; // 返回游戏对象
}

// 王室卫士宝珠对象函数实现
GameObject RoyalGuardOrb() {
    GameObject unit; // 创建游戏对象
    unit.sprite = sprites::yellow_orb; // 设置精灵图像
    unit.tags = SPELL; // 设置标签为法术
    unit.collisionMask = MOBILE | PLAYER; // 设置碰撞掩码为移动物体和玩家
    unit.hp = 1; // 设置生命值为 1
    unit.despawnOnBounce = true; // 设置碰撞反弹时消失
    unit.despawnOnCollision = true; // 设置碰撞时消失

    Damaging* damagingBehaviour = new Damaging(unit); 
    DespawnTimer* despawnTimerBehaviour = new DespawnTimer(unit, 3000);
    
    unit.addBehaviour(damagingBehaviour); // 添加造成伤害行为对象
    unit.addBehaviour(despawnTimerBehaviour); // 添加消失计时器行为对象（3 秒后消失）
    
    unit.friction = 0.9; // 设置摩擦系数

    return unit; // 返回游戏对象
}

// 巫师对象函数实现
GameObject Wizard() {
    GameObject unit = Villager(); // 创建村民对象
    unit.sprite = sprites::wizard; // 设置精灵图像
    unit.hp = unit.maxHp = 15; // 设置生命值和最大生命值
    unit.souls = 10; // 设置灵魂数量

    Summon* summonBehaviour = new Summon(unit, Portal, 3000);
    unit.addBehaviour(summonBehaviour); // 添加召唤行为对象（每 3 秒召唤一个传送门）

    return unit; // 返回游戏对象
}

// 传送门对象函数实现
GameObject Portal() {
    GameObject unit; // 创建游戏对象
    unit.sprite = sprites::portal; // 设置精灵图像
    unit.tags = LIVING; // 设置标签为生物
    unit.hp = unit.maxHp = 3; // 设置生命值和最大生命值
    unit.souls = 10; // 设置灵魂数量

    DespawnTimer* despawnTimerBehaviour = new DespawnTimer(unit, 3000 * 10);
    Summon* summonBehaviour = new Summon(unit, []() { return randomElement({Villager, Bandit, Archer})(); }, 3000);
    
    unit.addBehaviour(despawnTimerBehaviour); // 添加消失计时器行为对象（30 秒后消失）
    unit.addBehaviour(summonBehaviour); // 添加召唤行为对象（每 3 秒随机召唤一个村民、强盗或弓箭手）

    return unit; // 返回游戏对象
}

