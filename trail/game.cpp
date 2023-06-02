#include "game.h"

void GameObject::update(float dt) {
    onFrame(dt);

    updateClock -= dt;

    if (updateClock <= 0) {
        updateClock = updateSpeed;
        onUpdate();
    }

    if (emitter) {
        emitter->x = x;
        emitter->y = y;
    }
}

Behaviour* GameObject::addBehaviour(Behaviour* behaviour, int index) {
    auto it = std::find_if(behaviours.begin(), behaviours.end(), [&behaviour](const Behaviour* b) {
        return typeid(*b) == typeid(*behaviour);
    });

    if (it != behaviours.end()) return behaviour;

    if (index < 0 || index >= behaviours.size()) {
        behaviours.push_back(behaviour);
    } else {
        behaviours.insert(behaviours.begin() + index, behaviour);
    }

    behaviour->onAdded();
    return behaviour;
}

void GameObject::removeBehaviour(Behaviour* behaviour) {
    auto it = std::find(behaviours.begin(), behaviours.end(), behaviour);
    if (it == behaviours.end()) return;

    (*it)->onRemoved();
    behaviours.erase(it);
}

template <typename T>
T* GameObject::getBehaviour() {
    auto it = std::find_if(behaviours.begin(), behaviours.end(), [](const Behaviour* behaviour) {
        return typeid(*behaviour) == typeid(T);
    });

    if (it == behaviours.end()) return nullptr;
    return static_cast<T*>(*it);
}

void Particle::update(float dt) {
    x += vx * dt;
    y += vy * dt;
    life += dt;

    if (life > lifetime) {
        game.removeParticle(this);
    }
}

void Particle::draw() {
    sprite->draw(x, y, size);
}

void ParticleEmitter::update(float dt) {
    timer += dt;

    while (timer >= rate) {
        timer -= rate;

        for (int i = 0; i < count; i++) {
            auto particle = new Particle();
            particle->x = x;
            particle->y = y;
            particle->lifetime = lifetime;
            particle->size = size;
            particle->vy = -speed;

            float angle = (float)rand() / RAND_MAX * spread - spread / 2;
            particle->vx = sin(angle) * speed;

            game.addParticle(particle);
        }
    }
}

void ParticleEmitter::draw() {
    for (auto sprite : sprites) {
        sprite->setColor(color);
    }

    game.drawAdditive(additive);

    for (auto sprite : sprites) {
        sprite->draw(x, y);
    }

    game.drawAdditive(false);
}

void AnimatedSprite::draw(int x, int y, float scale) {
    int index = (int)(game.time * speed) % frames.size();

    if (!loop && index == frames.size() - 1) {
        index = frames.size() - 2;
    }

    frames[index]->draw(x, y, scale);
}

GameObject* Game::spawn(Sprite* sprite, int x, int y, float vx, float vy, float mass, float bounce, float friction, int tags, int collisionMask, int hp, int maxHp, int souls, float corpseChance, bool despawnOnCollision, bool despawnOnBounce, int groupId) {
    auto object = new GameObject();
    object->sprite.reset(sprite);
    object->x = x;
    object->y = y;
    object->vx = vx;
    object->vy = vy;
    object->mass = mass;
    object->bounce = bounce;
    object->friction = friction;
    object->tags = tags;
    object->collisionMask = collisionMask;
    object->hp = hp;
    object->maxHp = maxHp;
    object->souls = souls;
    object->corpseChance = corpseChance;
    object->despawnOnCollision = despawnOnCollision;
    object->despawnOnBounce = despawnOnBounce;
    object->groupId = groupId;

    objects.push_back(object);
    return object;
}

GameObject* Game::spawn(Sprite* sprite, Point pos, float vx, float vy, float mass, float bounce, float friction, int tags, int collisionMask, int hp, int maxHp, int souls, float corpseChance, bool despawnOnCollision, bool despawnOnBounce, int groupId) {
    return spawn(sprite, pos.x, pos.y, vx, vy, mass, bounce, friction, tags, collisionMask, hp, maxHp, souls, corpseChance, despawnOnCollision, despawnOnBounce, groupId);
}

void Game::despawn(GameObject* object) {
    for (int i = 0; i < objects.size(); i++) {
        if (objects[i] == object) {
            delete objects[i];
            objects.erase(objects.begin() + i);
            break;
        }
    }
}

void Game::addParticle(Particle* particle) {
    particles.push_back(particle);
}

void Game::removeParticle(Particle* particle) {
    for (int i = 0; i < particles.size(); i++) {
        if (particles[i] == particle) {
            delete particles[i];
            particles.erase(particles.begin() + i);
            break;
        }
    }
}

ParticleEmitter* Game::createEmitter(const std::vector<Sprite*>& sprites,
float rate,
float lifetime,
float size,
float speed,
float spread,
int count,
int color,
bool additive) {
auto emitter = new ParticleEmitter();
emitter->sprites = sprites;
emitter->rate = rate;
emitter->lifetime = lifetime;
emitter->size = size;
emitter->speed = speed;
emitter->spread = spread;
emitter->count = count;
emitter->color = color;
emitter->additive = additive;

emitters.push_back(emitter);
return emitter;
}

void Game::update(float dt) {
deltaTime = dt;
time += dt;

for (auto object : objects) {
object->update(dt);
}

for (auto emitter : emitters) {
emitter->update(dt);
}

for (auto particle : particles) {
particle->update(dt);
}
}

void Game::draw() {
for (auto object : objects) {
object->sprite->draw(object->x,    object->y);
}

for (auto emitter : emitters) {
    emitter->draw();
}

for (auto particle : particles) {
    particle->draw();
}
}

void Game::drawAdditive(bool on) {

}
