#pragma once

#include "game_object.h"
#include "game.h"
#include "emitter.h"

#include <functional>
#include <cmath>
#include<map>

constexpr int MAX_STREAK = 5;

// Attack
class Attack : public Behavior {
public:
    void onCollision(GameObject& object, GameObject& other) override;
};

// DespawnTimer
class DespawnTimer : public Behavior {
    float elapsed;
    float duration;
public:
    DespawnTimer(float duration);
    void onUpdate(GameObject& object, float dt) override;
};

// March
class March : public Behavior {
    float step;
public:
    March(float step);
    void onUpdate(GameObject& object, float dt) override;
};

// Damaging
class Damaging : public Behavior {
    int amount;
public:
    Damaging(int amount);
    void onCollision(GameObject& object, GameObject& other) override;
};

// Bleeding
class Bleeding : public Behavior {
    int turns;
    int amount;
    Emitter emitter{ Color::RED };
public:
    Bleeding(int turns, int amount);
    void onUpdate(GameObject& object, float dt) override;
};

// Enraged
class Enraged : public Behavior {
    unsigned int mask;
public:
    Enraged(unsigned int mask);
    void onDamage(GameObject& object, int amount, GameObject* dealer) override;
    std::function<void(GameObject*)> onEmitted;
};

// Seeking
class Seeking : public Behavior {
public:
    void onUpdate(GameObject& object, float dt) override;
};

// Summon
class Summon : public Behavior {
    float summonSpeed;
    float summonTimer = 0;
    int summonCounter = 0;
    GameObject* (*create)();
public:
    Summon(GameObject* (*create)(), float summonSpeed = 1);
    void onFrame(GameObject& object, float dt) override;
    std::function<void(GameObject&)> onSummon;
};

// HitStreak
class SpellCounter {
public:
    int total = 0;
    int hits = 0;
};

class HitStreak : public Behavior {
    static std::unordered_map<int, SpellCounter> counters;
    SpellCounter* counter;
public:
    bool hit = false;
    void onAdded(GameObject& object) override;
    void onRemoved(GameObject& object) override;
};

// Invulnerable
class Invulnerable : public Behavior {
public:
    void onDamage(GameObject& object, int amount, GameObject* dealer) override;
};

// Frozen
class Frozen : public Behavior {
    int freezeTimer;
public:
    Frozen(int freezeTimer);
    bool onUpdate(GameObject& object, float dt) override;
};

// LightningStrike
class LightningStrike : public Behavior {
public:
    void onCollision(GameObject& object, GameObject& other) override;
};
