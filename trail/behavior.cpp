#include "behavior.h"

// Attack
void Attack::onCollision(GameObject& object, GameObject& other) {
    auto damage = other.getBehavior<Damaging>();
    if (damage != nullptr) {
        object.destroy();
        damage->inflict();
    }
}

// DespawnTimer
DespawnTimer::DespawnTimer(float duration) : duration(duration) {}

void DespawnTimer::onUpdate(GameObject& object, float dt) {
    elapsed += dt;
    if (elapsed >= duration) {
        object.destroy();
    }
}

// March
March::March(float step) : step(step) {}

void March::onUpdate(GameObject& object, float dt) {
    object.vx = step * dt;
}

// Damaging
Damaging::Damaging(int amount) : amount(amount) {}

void Damaging::onCollision(GameObject& object, GameObject& other) {
    auto invulnerable = other.getBehavior<Invulnerable>();
    if (invulnerable != nullptr) {
        return;
    }
    auto health = other.getBehavior<Health>();
    if (health != nullptr) {
        health->damage(amount);
        auto hitStreak = object.getBehavior<HitStreak>();
        if (hitStreak != nullptr) {
            hitStreak->hit = true;
        }
        auto bleeding = other.getBehavior<Bleeding>();
        if (bleeding == nullptr && randf() < 0.5f) {
            other.addBehavior(new Bleeding(3, 5));
        }
    }
}

// Bleeding
Bleeding::Bleeding(int turns, int amount) : turns(turns), amount(amount) {}

void Bleeding::onUpdate(GameObject& object, float dt) {
    if (turns-- <= 0) {
        object.removeBehavior(this);
    }
    auto health = object.getBehavior<Health>();
    if (health != nullptr) {
        health->damage(amount);
        emitter.emit(object.x, object.y);
    }
}

// Enraged
Enraged::Enraged(unsigned int mask) : mask(mask) {}

void Enraged::onDamage(GameObject& object, int amount, GameObject* dealer) {
    auto hitStreak = object.getBehavior<HitStreak>();
    if (hitStreak == nullptr) {
        hitStreak = new HitStreak();
        object.addBehavior(hitStreak);
    }
    hitStreak->counter->hits++;
    if (hitStreak->counter->hits >= MAX_STREAK) {
        auto objects = object.pool->getObjects(mask);
        for (auto obj : objects) {
            auto behavior = obj->getBehavior<Enraged>();
            if (behavior != nullptr && behavior != this) {
                behavior->onEmitted(obj);
            }
        }
        object.pool->shake(5.0f, 1.0f);
        hitStreak->reset();
    }
}

std::function<void(GameObject*)> Enraged::onEmitted;

// Seeking
void Seeking::onUpdate(GameObject& object, float dt) {
    auto player = object.pool->getPlayer();
    if (player == nullptr) {
        return;
    }
    auto angle = atan2(player->y - object.y, player->x - object.x);
    object.vx = cos(angle) * object.speed;
    object.vy = sin(angle) * object.speed;
}

// Summon
Summon::Summon(GameObject* (*create)(), float summonSpeed) : create(create), summonSpeed(summonSpeed) {}

void Summon::onFrame(GameObject& object, float dt) {
    summonTimer += dt;
    if (summonTimer >= summonSpeed) {
        summonTimer -= summonSpeed;
        auto obj = create();
        obj->x = object.x + randf() * 10.0f - 5.0f;
        obj->y = object.y + randf() * 10.0f - 5.0f;
        object.pool->addObject(obj);
        if (onSummon != nullptr) {
            onSummon(*obj);
        }
        summonCounter++;
        if (summonCounter >= object.spell->maxCasts) {
            object.removeBehavior(this);
        }
    }
}

// HitStreak
std::unordered_map<int, SpellCounter> HitStreak::counters;

void HitStreak::onAdded(GameObject& object) {
    auto counter = counters.find(object.id);
    if (counter == counters.end()) {
        counters.emplace(object.id, SpellCounter{});
        counter = counters.find(object.id);
    }
    this->counter = &counter->second;
    counter->second.total++;
}

void HitStreak::onRemoved(GameObject& object) {
    counter->total--;
    if (counter->total <= 0) {
        counters.erase(object.id);
    }
}

// Invulnerable
void Invulnerable::onDamage(GameObject& object, int amount, GameObject* dealer) {}

// Frozen
Frozen::Frozen(int freezeTimer) : freezeTimer(freezeTimer) {}

bool Frozen::onUpdate(GameObject& object, float dt) {
    object.vx = 0;
    object.vy = 0;
    freezeTimer -= dt;
    return freezeTimer > 0;
}

// LightningStrike
void LightningStrike::onCollision(GameObject& object, GameObject& other) {
    auto health = other.getBehavior<Health>();
    if (health != nullptr) {
        health->damage(50);
    }
}
