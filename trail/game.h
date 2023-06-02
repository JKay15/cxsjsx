#ifndef GAME_H
#define GAME_H

#include <vector>
#include <memory>

#include<helpers.h>
#include<action.h>

enum State {
    INTRO,
    PLAYING,
    SHOPPING,
    LOSE,
    WIN
};

class Sprite {
public:
int w;
int h;

virtual void draw(int x, int y, float scale = 1.0f) {}
};

class GameObject;

class Behaviour {
public:
    virtual ~Behaviour(){}
    virtual void onAdded() {}
    virtual void onRemoved() {}
    virtual void onUpdate() {}
    virtual void onBounce() {}
    virtual void onDamage(Damage damage) {}
    virtual void onDeath(Death death) {}
    virtual void onFrame(float dt) {}
    virtual void onCollision(GameObject& target) {}
private:
    GameObject* object;
    int turns = 1;
    int timer = 0;
    std::unique_ptr<Sprite> sprite;
};

class GameObject {
public:
    // Physics
    int x = 0;
    int y = 0;
    float vx = 0;
    float vy = 0;
    float mass = 0;
    float bounce = 0;
    float friction = 0;
    int hop = 0;

    // Display
    std::unique_ptr<Sprite> sprite;
    ParticleEmitter* emitter = nullptr;

    // Logic
    int tags = 0;
    int collisionMask = 0;
    int hp = 0;
    int maxHp = 0;
    int souls = 0;
    float corpseChance = 0;
    bool despawnOnCollision = false;
    bool despawnOnBounce = false;
    int groupId = 0;

    // Behaviours
    std::vector<Behaviour*> behaviours;
    float updateSpeed = 0;
    float updateClock = 0;

    bool is(int mask) {
        return (tags & mask) > 0;
    }

    Rectangle bounds() {
        return Rectangle({x, y, sprite->w, sprite->h});
    }

    Point center() {
        return Point({x + sprite->w / 2, y + sprite->h / 2});
    }

    void update(float dt);
    Behaviour* addBehaviour(Behaviour* behaviour, int index = -1);
    void removeBehaviour(Behaviour* behaviour);
    template <typename T>
    T* getBehaviour();
};

class Damage {
public:
    int amount;
GameObject* source;
};

class Death {
public:
GameObject* source;
};

class Particle {
public:
int x;
int y;
float vx;
float vy;
float life;
float lifetime;
float size;
Sprite* sprite;

void update(float dt);
void draw();
};

class ParticleEmitter {
public:
std::vector<Sprite*> sprites;
float rate;
float lifetime;
float size;
float speed;
float spread;
int count;
int color;
bool additive;
int x = 0;
int y = 0;

void update(float dt);
void draw();
private:
float timer = 0;
};



class AnimatedSprite : public Sprite {
public:
std::vector<Sprite*> frames;
float speed;
bool loop;

void draw(int x, int y, float scale = 1.0f) override ;
};

class Game {
public:
std::vector<GameObject*> objects;
std::vector<Particle*> particles;
std::vector<ParticleEmitter*> emitters;

float time = 0;
float deltaTime = 0;

GameObject* spawn(Sprite* sprite, int x, int y, float vx = 0, float vy = 0, float mass = 0, float bounce = 0, float friction = 0, int tags = 0, int collisionMask = 0, int hp = 0, int maxHp = 0, int souls = 0, float corpseChance = 0, bool despawnOnCollision = false, bool despawnOnBounce = false, int groupId = 0);
GameObject* spawn(Sprite* sprite, Point pos, float vx = 0, float vy = 0, float mass = 0, float bounce = 0, float friction = 0);
        int tags = 0;
        int collisionMask = 0;
        ParticleEmitter* createEmitter(const std::vector<Sprite*>& sprites, float rate, float lifetime, float size, float speed, float spread, int count, int color, bool additive = false);
        void update(float dt);
        void draw();
        void drawAdditive(bool on);
    };

    #endif // GAME_H
