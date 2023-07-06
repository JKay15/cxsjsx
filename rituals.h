#ifndef RITUALS_H
#define RITUALS_H
#include "behaviours.h"
#include "game.h"
#include "sprites.h"
#include "engine.h"
#include "helpers.h"
#include "objects.h"
#include "tags.h"
#include "shop.h"
#include "fx.h"
#include <random>

// Ritual tags
const int NONE = 0;
const int BOUNCING = 1 << 0;
const int SPLITTING = 1 << 1;
const int EXPLOSIVE = 1 << 2;
const int HOMING = 1 << 3;
const int WARDSTONES = 1 << 4;
const int CASTING_RATE = 1 << 5;
const int CURSE = 1 << 6;

Ritual Streak{
    NONE,
    "Streak",
    "",
    [](GameObject* spell){
        spell->addBehaviour(new HitStreak(spell));
    }
};

Ritual Bouncing{
    BOUNCING,
    "Bouncing",
    "Spells bounce",
    [](GameObject* spell){
        spell->addBehaviour(new DespawnTimer(*spell,3000));

    }
};
Ritual Doubleshot{
    SPLITTING,
    SPLITTING,
    RARE,
    "Doubleshot",
    "Cast 2 spells",
    [](){
        game.spell.shotsPerRound=2;
    }
};
Ritual Hunter{
    HOMING,
    RARE,
    "Hunter",
    "Spells seek targets",
    [](GameObject* projectile){
        projectile->addBehaviour(new Seeking(projectile));
    }
};
Ritual Weightless{
    NONE,
    "Weightless",
    "Spells are not affected by gravity",
    [](GameObject* spell){
        spell->mass=0;
        spell->friction=0;
        spell->bounce=1;
    }
};

class KnockbackSpell:public Behaviour{
public:
    KnockbackSpell(GameObject* ob):Behaviour(ob){
        onCollision=[=](GameObject* target){
            if(target->mass<1000){
                tween(target->x,target->x+16,200,[=](double v,double t){target->x=v*t;});
            }
        };
    }
};

Ritual Knockback{
    NONE,
    "Knockback",
    "Spells knock backwards",
    [](GameObject* spell){
        spell->addBehaviour(new KnockbackSpell(spell));
    }
};
Ritual Ceiling{
    NONE,
    BOUNCING,
    "Ceiling",
    "Adds a ceiling",
    [](){
        game.stage.ceiling=48;
    }
};
class RainSpell:public Behaviour{
public:
    bool split=false;
    std::function<void()> onFrame;
    RainSpell(GameObject* ob):Behaviour(ob){
        onFrame=[=](){
            if(!split&&object->vy<0){
                split=true;
                GameObject* p0=object;
                GameObject p1= objects::Spell();
                GameObject p2= objects::Spell();
                p1.x = p2.x = p0->x;
                p1.y = p2.y = p0->y;
                p1.vx = p2.vx = p0->vx;
                p1.vy = p2.vy = p0->vy;
                p1.vx -= 20;
                p2.vx += 20;
                p1.groupId = p2.groupId = p0->groupId;
                game.onCast(&p1, true);
                game.onCast(&p2, true);
                game.spawn(p1);
                game.spawn(p2);
            }
        };
    }
};

Ritual Rain{
    SPLITTING,
    SPLITTING,
    RARE,
    "Rain",
    "Spells split when they drop",
    false,
    [](GameObject* spell){
        spell->addBehaviour(new RainSpell(spell));
    }
};
Ritual Drunkard{
    NONE,
    "Drunkard",
    "2x damage, wobbly aim",
    [](GameObject* spell){
        spell->vx += randomInt(100) - 50;
        spell->vy += randomInt(100) - 50;
        std::optional<Damaging*> behaviourPtr = spell->getBehaviour<Damaging>();
        if(behaviourPtr){
            Damaging* Damageptr=*behaviourPtr;
            Damageptr->amount*=2;
        }
    }
};
Ritual Seer{
    NONE,
    "Seer",
    "Spells pass through the dead",
    [](GameObject* spell){
        spell->collisionMask=LIVING;
    }
};
Ritual Tearstone{
    NONE,
    "Tearstone",
    "2x damage when < half HP",
    [](GameObject* spell){
        if(game.player->hp<game.player->maxHp/2){
            std::optional<Damaging*> behaviourPtr = spell->getBehaviour<Damaging>();
            if(behaviourPtr){
                Damaging* Damageptr=*behaviourPtr;
                Damageptr->amount*=3;
            }
        }
    }
};
Ritual Impatience{
    NONE,
    "Impatience",
    "Resurrection recharges 2x faster",
    [](){
        game.ability.cooldown/=2;
    }
};
Ritual Bleed{
    CURSE,
    "Bleed",
    "Inflicts bleed on hits",
    [](GameObject* spell){
        spell->sprite=sprites::p_red_skull;
        spell->emitter.extend({
                                  {sprites::p_red_3, sprites::p_red_2, sprites::p_red_1},
                                  {sprites::p_red_4, sprites::p_red_3, sprites::p_red_2},
                                  {sprites::p_red_3, sprites::p_red_2, sprites::p_red_1}
                              },
                              5,
                              {DEG_180, 0},
                              {20,50}
                              );
        Behaviour* inflict=spell->addBehaviour();
        inflict->onCollision=[&](GameObject* target){
            target->addBehaviour(new Bleeding(target));
        };
    }
};
Ritual Allegiance{
    NONE,
    "Allegiance",
    "Summon your honour guard after resurrections",
    [](){
        for(int i=0;i<3;i++){
            GameObject unit=objects::SkeletonLord();
            unit.updateSpeed=200;
            game.spawn(unit,i*-15,0);
        }
    }
};
Ritual Salvage{
    NONE,
    "Salvage",
    "Corpses become souls at the end of levels",
    [](){
        std::vector<GameObject> corpses;
        std::copy_if(game.objects.begin(), game.objects.end(), std::back_inserter(corpses),
            [](const GameObject& object) { return object.is(CORPSE); });
        for(GameObject corpse:corpses){
            ParticleEmitter emitter=fx::bones().extend(
                        corpse.center().x,
                        corpse.center().y,
                        {{sprites::p_green_skull}},
                        {100,1000});
            emitter.burst(5);
            emitter.remove();
            game.spawn(corpse);
            game.addSouls(5);
        }
    }
};

Ritual Studious{
    NONE,
    RARE,
    "Studious",
    "Rituals are 50% cheaper",
    [](){
        for(auto item: shop.items){
            item.cost=static_cast<int>(item.cost / 2);
        }
    }
};
Ritual Electrodynamics{
    NONE,
    RARE,
    "Electrodynamics",
    "Lightning strikes after hits",
    [](GameObject* spell){
        spell->addBehaviour(new LightningStrike(spell));
    }
};

Ritual Chilly{
    NONE,
    "Chilly",
    "10% chance to freeze enemies",
    [](GameObject* spell){
        if(randomFloat()<0.1){
            spell->emitter.variants={{sprites::p_ice_1, sprites::p_ice_2, sprites::p_ice_3}};
            spell->sprite=sprites::p_skull;
            std::optional<Damaging*> behaviourPtr = spell->getBehaviour<Damaging>();
            if(behaviourPtr){
                Damaging* Damageptr=*behaviourPtr;
                Damageptr->amount=0;
            }
            spell->addBehaviour()->onCollision=[&](GameObject* target){
                if(target->mass<1000){
                    target->addBehaviour(new Frozen(target),0);
                }
            };
        }
    }
};
Ritual Giants{
    NONE,
    "Giants",
    "20% chance to resurrect giant skeletons",
    [](GameObject* Object1){
        if(randomFloat()<0.2){
            game.despawn(Object1);
            game.spawn(objects::SkeletonLord(),Object1->x,Object1->y);
        }
    }
};
Ritual Hardened{
    NONE,
    "Hardened",
    "Undead have +1 HP*",
    [](GameObject* object){
        object->hp=object->maxHp+=1;
    }
};
#endif // RITUALS_H
