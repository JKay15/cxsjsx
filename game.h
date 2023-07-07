#ifndef GAME_H
#define GAME_H
#include "helpers.h"
#include "engine.h"
#include "tags.h"
#include "actions.h"
#include <cmath>
#include<optional>
#include <functional>

const int MAX_STREAK=10;
class Game;
extern Game game;
struct Window{
    static Game* game;
}window;

enum Rarity{
    COMMON=0,
    RARE=1
};
class GameObject;
//伤害的结构体
struct Damage {
  int amount;
  GameObject* dealer;
};
//死亡的结构体
struct Death {
  GameObject* object;
  GameObject* killer;
  int souls;
};
//舞台stage的结构体
struct Stage{
    double width;
    double height;
    double floor;
    double ceiling;
};
//spell的结构体
struct Spell {
  double targetAngle;
  double targetRadius;
  double basePower;
  int shotsPerRound;
  double shotOffsetAngle;
  int maxCasts;
  int casts;
  double castRechargeRate;
  double castRechargeTimer;
};
//能力的结构体
struct Ability {
  double cooldown;
  double timer;
};
//ritual的结构体
struct Ritual {
  std::string name;
  std::string description;
  int tags;
  int exclusiveTags;
  int requiredTags;
  bool recursive;
  Rarity rarity;
  void (*onFrame)(float dt);
  std::function<void()> onActive;
  std::function<void(GameObject*)> onCast;
  std::function<void()> onResurrect;
  std::function<void(GameObject*)> onResurrection;
  void (*onDeath)(Death death);
  std::function<void()> onLevelEnd;
  void (*onLevelStart)();
  std::function<void()> onShopEnter;
  Ritual(int Tags,std::string Name,std::string Description,std::function<void(GameObject*)> tmp):name(Name),description(Description),tags(Tags){
      if(Name=="Giants"||Name=="Hardened"){
          onResurrection=tmp;
      }else{
          onCast=tmp;
      }
  };
  Ritual(int Tags,int ExclusiveTags,Rarity Rarity,std::string Name,std::string Description,std::function<void()> OnActive):name(Name),description(Description),tags(Tags),exclusiveTags(ExclusiveTags),rarity(Rarity),onActive(OnActive){};
  Ritual(int Tags,Rarity Rarity,std::string Name,std::string Description,std::function<void(GameObject*)> OnCast):name(Name),description(Description),tags(Tags),rarity(Rarity),onCast(OnCast){};
  Ritual(int Tags,int RequiredTags,std::string Name,std::string Description,std::function<void()> OnActive):name(Name),description(Description),tags(Tags),requiredTags(RequiredTags),onActive(OnActive){};
  Ritual(int Tags,int ExclusiveTags,Rarity Rarity,std::string Name,std::string Description,bool Recursive,std::function<void(GameObject*)> OnCast):name(Name),description(Description),tags(Tags),exclusiveTags(ExclusiveTags),recursive(Recursive),rarity(Rarity),onCast(OnCast){};
  Ritual(int Tags,std::string Name,std::string Description,std::function<void()> tmp):name(Name),description(Description),tags(Tags){
      if(Name=="Impatience"){
          onActive=tmp;
      }else if(Name=="Allegiance"){
          onResurrect=tmp;
      }else if(Name=="Salvage"){
          onLevelEnd=tmp;
      }else if(Name=="Studious"){
          onShopEnter=tmp;
      }
  };

};
//一些常量
const int INTRO = 0;
const int PLAYING = 1;
const int SHOPPING = 2;
const int LOSE = 3;
const int WIN = 4;

typedef int State;
//商店物品
struct ShopItem {
  int cost;
  std::string name;
  std::string description;
  std::function<void()> purchase;
  ShopItem(){}
  ShopItem(int cost1, const std::string& name1, const std::string& description1, std::function<void()> purchase1):cost(cost1),name(name1),description(description1),purchase(purchase1){}
};
ShopItem createShopItem(int cost, const std::string& name, const std::string& description, void (*purchase)()) {
  ShopItem item;
  item.cost = cost;
  item.name = name;
  item.description = description;
  item.purchase = purchase;
  return item;
}

//行为的类
class Behaviour{
  public:
    Behaviour(GameObject* object) : object(object) {}
    int turns = 1;
    int timer = 0;
    Sprite sprite;

    std::function<void()> onAdded;
    std::function<void()> onRemoved;
    std::function<std::optional<bool>()> onUpdate;
    std::function<void()> onBounce;
    std::function<void(struct Damage*)> onDamage;
    std::function<void(Death)> onDeath;
    std::function<void(double)> onFrame;
    std::function<void(GameObject*)> onCollision;
    GameObject* object;
};

//游戏物体的类
class GameObject{
public:
    //物理变量
    double x=0;
    double y=0;
    double vx=0;
    double vy=0;
    double mass=0;
    double bounce=0;
    double friction=0;
    double hop=0;
    //显示变量
    Sprite sprite={0,0,0,0};
    ParticleEmitter emitter;
   //逻辑变量
    int tags=0;
    int collisionMask=0;
    double hp=0;
    double maxHp=0;
    int souls=0;
    double corpseChance=0;
    bool despawnOnCollision=0;
    bool despawnOnBounce = 0;
    int groupId=0;

    //行为
    std::vector<Behaviour*> behaviours;
    double updateSpeed=0;
    double updateClock = 0;

    virtual bool is(int mask)const{
        return (tags & mask) > 0;
    }
    virtual Rectangle bounds(){
        return Rectangle(x, y, sprite[2], sprite[3]);
    }
    virtual Point center(){
        return  Point(x + sprite[2] / 2, y + sprite[3] / 2);
    }
    virtual void onFrame(int dt){
        for(auto behaviour:behaviours){
             behaviour->onFrame(dt);
        }
    }
    virtual void onUpdate(){
        for(auto behaviour:behaviours){
             if(++behaviour->timer>=behaviour->turns){
                 behaviour->timer=0;
                 if(behaviour->onUpdate())break;
             }
        }
    }
    virtual void onDamage(Damage& damage){
        for(auto behaviour:behaviours){
             behaviour->onDamage(&damage);
        }
    }
    virtual void onDeath(Death& death){
        for(auto behaviour:behaviours){
             behaviour->onDeath(death);
        }
    }
    virtual void onBounce(){
        for(auto behaviour:behaviours){
             behaviour->onBounce();
        }
        if(despawnOnBounce){

        }
    }
    virtual void onCollision(GameObject* target);
    virtual void removeBehaviour(Behaviour* behaviour){
        auto it=std::find(behaviours.begin(),behaviours.end(),behaviour);
        if(it!=behaviours.end()){
            behaviours.erase(it);
            (*it)->onRemoved();
        }
    }
    virtual void update(int dt){
        onFrame(dt);
        updateClock-=dt;
        if(updateClock<=0){
            updateClock=updateSpeed;
            onUpdate();
        }
        emitter.x=x;
        emitter.y=y;
    }
    virtual Behaviour* addBehaviour(Behaviour* behaviour = nullptr,int index=-1) {
      if(index==-1)
        index = behaviours.size();
      if (behaviour == nullptr) {
        behaviour = new Behaviour(this);
      }

      for (Behaviour* existingBehaviour : behaviours) {
        if (typeid(*existingBehaviour) == typeid(*behaviour)) {
          return behaviour;
        }
      }

      behaviours.insert(behaviours.begin() + index, behaviour);
      behaviour->onAdded();
      return behaviour;
    }
    template<typename T>
    std::optional<T*> getBehaviour() {
      for (Behaviour* behaviour : behaviours) {
        if (dynamic_cast<T*>(behaviour)) {
          return dynamic_cast<T*>(behaviour);
        }
      }
      return nullptr;
    }
};
class Game{
public:
    Stage stage={400, 200, 0, 200};
    std::vector<GameObject*> objects;
    GameObject* player;
    std::vector<Ritual*> rituals;
    State state=INTRO;
    int souls=0;
    int streak=0;
    int level=0;
    std::vector<std::string> dialogue;
    Spell spell={0,15,180,1,0.1,3,3,1000,0};
    Ability ability={10000,10000};
    void spawn(GameObject object1,int x=0,int y=0) {
        if(x!=object1.x||y!=object1.y){
            object1.x = x;
            object1.y = y;
        }
        objects.push_back(&object1);
    }
    void despawn(GameObject* object) {
      object?object->emitter.remove():void();
      for (auto it = object->behaviours.begin(); it != object->behaviours.end(); ++it) {
        object->removeBehaviour(*it);
      }
      removeFromArray(objects,object);
    }

    Game(GameObject player1){
        player=&player1;
        spawn(*player);
        window.game=this;
    }
    double getStreakMultiplier() {
        return streak/MAX_STREAK;
    }

    void addSouls(int amount){
        souls+=amount+amount*getStreakMultiplier();
    }

    void addRitual(Ritual* ritual){
        rituals.push_back(ritual);
        ritual ? ritual->onActive() : void();
    }
    bool canAddRitual(Ritual ritual){
        if(ritual.exclusiveTags){
            for(auto other: rituals){
                if(ritual.exclusiveTags&other->exclusiveTags){
                    return false;
                }
            }
        }
        if(ritual.requiredTags){
            for(auto other:rituals){
                if(ritual.requiredTags&other->tags){
                    return true;
                }
            }
            return false;
        }
        return true;
    }
    void onLevelStart(){
        for(auto ritual:game.rituals){
            ritual?ritual->onLevelStart():void();
        }
    }
    void onLevelEnd(){
        for(auto ritual:game.rituals){
            ritual?ritual->onLevelEnd():void();
        }
    }
    void onShopEnter(){
        for(auto ritual:game.rituals){
            ritual?ritual->onShopEnter():void();
        }
    }
    void onCast(GameObject* spell,bool recursive=false){
        for(auto ritual:game.rituals){
            if(recursive&&ritual->recursive==false){
                continue;
            }
            ritual?ritual->onCast(spell):void();
        }
    }
    Point getCastingPoint(){
        double targetX=player->center().x+cos(spell.targetAngle)*spell.targetRadius;
        double targetY=player->center().y+sin(spell.targetAngle)*spell.targetRadius;
        return {targetX,targetY};
    }
    void update(int dt){
        updateAbility(dt);
        updateSpell(dt);
        updateObjects(dt);
        updatePhysics(dt);
        updateRituals(dt);
    }
private:
    void updateAbility(int  dt){
        game.ability.timer+=dt;
        game.player->emitter.frequency=game.ability.timer>=game.ability.cooldown? 0.1:0;
    }
    void updateSpell(int dt){
        if(spell.casts<spell.maxCasts){
            spell.castRechargeTimer+=dt;
            if(spell.castRechargeTimer>spell.castRechargeRate){
                spell.casts+=1;
                spell.castRechargeTimer=0;
            }
        }
    }
    void updateRituals(int dt){
        for(auto ritual:rituals){
            ritual?ritual->onFrame(dt):void();
        }
    }
    void updateObjects(int dt){
        for(auto object:objects){
            object->update(dt);
        }
    }
    void updatePhysics(int dt){
        double d=dt/1000;
        //速度
        for(auto object:objects){
            object->x+=object->vx*d;
            object->y+=object->vy*d;
        }
        //反弹
        for(auto object:objects){
            double lower=stage.floor;
            double upper=stage.ceiling-object->sprite[3];
            if(object->y<lower||object->y>upper){
                object->y=clamp(object->y,lower,upper);

                if(abs(object->vy)>=10){
                    object->onBounce();
                }
                object->vy*=-object->bounce;
            }
            if(object->y==lower||object->y==upper){
                object->vx*=(1-object->friction);
            }
            if(object->mass&&object->y>0){
                object->vy-=object->mass*d;
            }
        }
        //碰撞
        for(auto object:objects){
            for(auto target:objects){
                if(object->collisionMask&target->tags){
                    if(overlaps(object->bounds(),target->bounds())){
                        object->onCollision(target);
                    }
                }
            }
        }
    }

};
void GameObject::onCollision(GameObject *target){
    for(auto behaviour:behaviours){
        behaviour->onCollision(target);
    }
    if(despawnOnCollision){
        game.despawn(this);
    }
}

#endif // GAME_H
