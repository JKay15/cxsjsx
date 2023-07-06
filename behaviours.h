#ifndef BEHAVIOURS_H
#define BEHAVIOURS_H
#include <string>
#include "game.h"
#include "helpers.h"
#include "objects.h"
#include "tags.h"
#include "sprites.h"
#include "engine.h"
#include "actions.h"
#include "sprites.h"
#include "fx.h"

// 攻击行为类
class Attack : public Behaviour {
public:
    Attack(GameObject* object):Behaviour(object){
        onCollision = [&](GameObject* target) { // 使用引用捕获方式
                    int dealDamage = object->hp;
                    int takeDamage = target->hp;
                    actions::Damage(target, dealDamage, object);
                    actions::Damage(object, takeDamage, target);
         };
    }
    // 碰撞处理函数
};

// 消失计时器行为类
class DespawnTimer : public Behaviour {
public:
    int elapsed = 0; // 已经过去的时间
    int duration; // 持续时间
    DespawnTimer(GameObject& object,const int duration) : Behaviour(&object), duration(duration) {
        onFrame=[&](double dt){
            if((elapsed+=dt)>=duration){
                game.despawn(this->object);
            }
        };
    }  // 构造函数
     // 帧更新函数
};

// 行军行为类
class March : public Behaviour {
public:
    int step; // 步长
    std::function<std::optional<bool>(QPainter& painter)> onUpdate;
    March(GameObject& object, int step):Behaviour(&object),step(step){
        onUpdate = [&](QPainter& painter) {
            if (this->object->y > 0) return std::optional<bool>();

            tween(this->object->x, this->object->x + step, 200, [&](double x, double t) {
                this->object->x = x;
                this->object->hop = std::sin(t * M_PI) * 2;
                if (t == 1 && this->object->mass >= 100) screenshake(painter,50);
            });

            if ((step < 0 && this->object->x < 0) || (step > 0 && this->object->x > game.stage.width)) {
                game.despawn(this->object);
            }

            return std::optional<bool>();
        };
    }; // 构造函数
    // 更新函数
};

// 造成伤害行为类
class Damaging : public Behaviour {
public:
    int amount = 1; // 伤害量 // 碰撞处理函数
    Damaging(GameObject* object):Behaviour(object){
         onCollision=[&](GameObject* target){
             actions::Damage(target, this->amount, this->object);
         };
    }
};

// 流血行为类
class Bleeding : public Behaviour {
public:
    std::vector<int> sprite = sprites::status_bleeding; // 精灵图像
    int turns = 3; // 持续回合数
    int amount = 1; // 伤害量

    ParticleEmitter emitter = fx::cloud({0, 0, 0, 0}, {{sprites::health_orb, sprites::health_pip},{sprites::health_pip}})
            .extend(0,{},{10,30},{10,30}); // 特效发射器

    Bleeding(GameObject* ob):Behaviour(ob){
        onUpdate=[&](){
            emitter.extend(object->center().x,object->center().y);
            emitter.burst(1);
            actions::Damage(object, 1, object);
            return std::optional<bool>();
        };
    }
};

// 愤怒行为类
class Enraged : public Behaviour {
public:
    std::vector<int> sprite = sprites::status_enraged; // 精灵图像

    ParticleEmitter emitter = fx::cloud({0, 0, 0, 0}, {{sprites::health_orb, sprites::health_pip}, {sprites::health_pip}})
        .extend(0,{},{10,30},{10,30}); // 特效发射器

    int mask; // 掩码

    Enraged(GameObject& object, int mask):Behaviour(&object),mask(mask){
        onDamage=[&](struct Damage* damage){
            if (damage->dealer && damage->dealer->is(this->mask)) {
                  actions::Damage(this->object, -damage->amount, this->object);
                  damage->amount = 0;
                  this->emitter.extend(this->object->bounds()).burst(4);
              }
        };
    } // 构造函数
     // 受到伤害处理函数
};

// 寻找行为类
class Seeking : public Behaviour {
public:
    std::function<void()> onFrame;
    Seeking(GameObject* ob):Behaviour(ob){
        onFrame = [&]() {
                    GameObject* projectile = object;
                    GameObject* target = nullptr;
                    double minDist = 100;

                    for (GameObject* obj : game.objects) {
                        if (obj->is(object->collisionMask)) {
                            double dist = distance(projectile->center(), obj->center());
                            if (dist < minDist) {
                                target = obj;
                                minDist = dist;
                            }
                        }
                    }

                    if (target) {
                        double currentAngle = vectorToAngle(projectile->vx, projectile->vy);
                        double desiredAngle = angleBetweenPoints(projectile->center(), target->center());
                        double angle = currentAngle + (desiredAngle - currentAngle) / 20;
                        double magnitude = std::hypot(projectile->vx, projectile->vy);
                        double vx, vy;
                        auto res = vectorFromAngle(angle);
                        vx=res[0];
                        vy=res[1];
                        projectile->vx = vx * magnitude;
                        projectile->vy = vy * magnitude;
                    }
     };
    }
};

// 召唤行为类
class Summon : public Behaviour {
private:
    int summonTimer = 0; // 召唤计时器
public:
    int summonCounter = 0; // 召唤计数器

    std::function<GameObject()> create; // 创建对象函数
    int summonSpeed; // 召唤速度
    std::function<void(GameObject*)> onSummon;
    void defaultOnSummon(GameObject* ob) {
            // 默认的 onSummon 行为
    }
    Summon(GameObject& object, std::function<GameObject()> create, int summonSpeed):Behaviour(&object),create(create),summonSpeed(summonSpeed){
        // 将默认函数绑定到 onSummon
        onSummon = std::bind(&Summon::defaultOnSummon, this, std::placeholders::_1);
        onFrame=[&](double dt){
            if((this->summonTimer+=dt)>this->summonSpeed){
                summonTimer=0;
                summonCounter++;
                GameObject ob=this->create();
                game.spawn(ob,this->object->x,this->object->y);
                this->onSummon(&ob);
            }
        };
    } // 构造函数
    void changeOnSummon(std::function<void()> newOnSummon) {
           onSummon = std::bind([](std::function<void()> func, GameObject* ob) {
               func();
           }, newOnSummon, std::placeholders::_1);
     }
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
    std::function<void()> onCollision=[&](){
        this->hit=true;
    };
    HitStreak(GameObject* ob):Behaviour(ob){
        onAdded=[=](){
            counter = HitStreak::counters[object->groupId];
            counter.total++;
        };
        onRemoved=[=](){
            if (hit) counter.hits++;
            if (--counter.total) return;
            if (counter.hits) {
                game.streak = clamp(game.streak + 1, 0, MAX_STREAK);
            } else {
                game.streak = 0;
            }
        };
    }

};
std::map<int, SpellCounter> HitStreak::counters;

// 无敌行为类
class Invulnerable : public Behaviour {
public:
    std::vector<int> sprite = sprites::status_shielded; // 精灵图像

    Invulnerable(GameObject* ob):Behaviour(ob){
        onDamage=[&](struct Damage* damage){
            if (damage->amount > 0) damage->amount = 0;
        };
    }
};

// 冰冻行为类
class Frozen : public Behaviour {
public:
    int freezeTimer = 10; // 冰冻计时器

    Frozen(GameObject* ob):Behaviour(ob){
        onUpdate=[&](){
            if (freezeTimer-- <= 0) {
                  object->removeBehaviour(this);
                }
            return true;
        };
    }
};

// 闪电打击行为类
class LightningStrike : public Behaviour {
public:
    LightningStrike(GameObject* ob):Behaviour(ob){
        onCollision=[&](GameObject* target){
            int bolts=3;
            for(int i=0;i<bolts;i++){
                auto bolt= objects::LightningSpell();
                bolt.vy = -200;
                 bolt.vx = randomInt(20) - 10;
                 bolt.y = clamp(50 + randomInt(100), 0, game.stage.ceiling - 10);
                 bolt.x = target->x + randomInt(50) - 25;
                 game.spawn(bolt);
            }
        };
    }
};
#endif // BEHAVIOURS_H
