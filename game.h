#include <vector>
#include <string>
#include "Sprite.h"
#include "ParticleEmitter.h"

class GameObject;
class Behaviour;
class Game;

typedef int Rarity;
const int COMMON = 0;
const int RARE = 1;

struct Damage {
    int amount;
    GameObject* dealer;
};

struct Death {
    GameObject* object;
    GameObject* killer;
    int souls;
};

struct Stage {
    int width;
    int height;
    int floor;
    int ceiling;
};

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

struct Ability {
    double cooldown;
    double timer;
};

struct Ritual {
    std::string name;
    std::string description;
    int tags;
    int exclusiveTags = 0;
    int requiredTags = 0;
    bool recursive = false;
    Rarity rarity = COMMON;

    void (*onFrame)(double dt) = nullptr;
    void (*onActive)() = nullptr;
    void (*onCast)(GameObject* spell) = nullptr;
    void (*onResurrect)() = nullptr;
    void (*onResurrection)(GameObject* object) = nullptr;
    void (*onDeath)(Death death) = nullptr;
    void (*onLevelEnd)() = nullptr;
    void (*onLevelStart)() = nullptr;
    void (*onShopEnter)() = nullptr; 
};

const int INTRO = 0; 
const int PLAYING = 1; 
const int SHOPPING = 2; 
const int LOSE = 3; 
const int WIN = 4; 
typedef int State;

struct ShopItem {
  int cost; 
  std::string name; 
  std::string description; 
  void (*purchase)(); 
};

ShopItem createShopItem(int cost, std::string name, std::string description, void (*purchase)()) {
  ShopItem item{cost, name, description, purchase};
  return item;
}

class GameObject {
public:
    // Physics
    double x = 0;
    double y = 0;
    double vx = 0;
    double vy = 0;
    double mass = 0;
    double bounce = 0;
    double friction = 0;
    double hop = 0;

    // Display
    Sprite sprite = {0, 0, 0, 0};
    ParticleEmitter* emitter;

    // Logic
    int tags = 0;
    int collisionMask = 0;
    int hp = 0;
    int maxHp = 0;
    int souls = 0;
    int corpseChance = 0;
    bool despawnOnCollision = false;
    bool despawnOnBounce = false;
    int groupId = 0;

    // Behaviours
    std::vector<Behaviour*> behaviours;
    int updateSpeed = 0;
    int updateClock = 0;

    bool is(int mask);
    Rectangle bounds();
    Point center();
    void update(int dt);
    Behaviour* addBehaviour(Behaviour* behaviour, int index);
    void removeBehaviour(Behaviour* behaviour);
    
private:
    void onFrame(int dt);
    void onUpdate();
};

class Behaviour {
public:
    GameObject* object;
    
public:
    Behaviour(GameObject* object) : object(object) {}
    
public:
    int turns = 1;
    int timer = 0;
    
public:
    Sprite sprite;

public:
     virtual void onAdded() {}
     virtual void onRemoved() {}
     virtual bool onUpdate() { return false; }
     virtual void onBounce() {}
     virtual void onDamage(Damage damage) {}
     virtual void onDeath(Death death) {}
     virtual void onFrame(int dt) {}
     virtual void onCollision(GameObject* target) {}
};

class Game {
public:
   Stage stage{400, 200, 0, 200};
   std::vector<GameObject*> objects;
   GameObject* player;
   std::vector<Ritual> rituals;
   State state = INTRO;
   int souls = 0;
   int streak = 0;
   int level = 0;
   std::vector<std::string> dialogue;

public:
   Spell spell{
       .targetAngle=0,
       .targetRadius=15,
       .basePower=180,
       .shotsPerRound=1,
       .shotOffsetAngle=0.1,
       .maxCasts=3,
       .casts=3,
       .castRechargeRate=1000,
       .castRechargeTimer=0
   };

public:
   Ability ability{
       .cooldown=10_000,
       .timer=10_000
   };

public:
   Game(GameObject* player) : player(player) {
       spawn(player);
   }

public:
   void spawn(GameObject* object, double x, double y);
   void despawn(GameObject* object);
   double getStreakMultiplier();
   void addSouls(int amount);
   void addRitual(Ritual ritual);
   bool canAddRitual(Ritual ritual);
   void update(int dt);

private:
   void updateAbility(int dt);
   void updateSpell(int dt);
   void updateObjects(int dt);
   void updatePhysics(int dt);
   void updateRituals(int dt);

public:
    void onLevelStart();
    void onLevelEnd();
    void onShopEnter();
    void onCast(GameObject* spell, bool recursive = false);
    Point getCastingPoint();
};
