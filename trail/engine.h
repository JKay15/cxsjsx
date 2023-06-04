// engine.h
#ifndef ENGINE_H
#define ENGINE_H
#pragma once

#include <set>
#include <utility>
#include <QPixmap>
#include "helpers.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <QRect>
#include <QJsonObject>


// Alias for Sprite
using Sprite = std::vector<int>;

extern QPixmap spriteSrc;
extern QImage spritesImage;
extern int glyphWidth;
extern int glyphHeight;
extern int lineHeight;
extern QJsonObject glyphWidthOverrides;
extern std::unordered_map<char, int> metrics;

//import font
void import_font();

// Rectangle class
class Rect {
public:
    double x;
    double y;
    double w;
    double h;

    Rect(double x, double y, double w, double h)
        : x(x), y(y), w(w), h(h) {}
};

//canvas methods
extern QImage canvas;
void clear();
void drawSprite(const Sprite& sprite, int x, int y);
void drawSceneSprite(const Sprite& sprite, int x, int y);
void drawSpriteSlice(const Sprite& sprite, int dx, int dy, int dw, int dh);
void drawNineSlice(const Sprite& sprite, int x, int y, int w, int h);

//write text
extern int textX;
extern int textY;
void write(const std::string& text, int x = textX, int y = textY);

//resize
void resize();

//init
void init(int width, int height, void (*update)(double));

//easing
using Easing = std::function<double(double)>;

//Tween
struct Tween {
  double startValue;
  double endValue;
  double duration;
  double elapsed;
  Easing ease;
  std::function<void(double, double)> callback;
};

extern std::vector<Tween> tweens;

extern double linear(double x);

extern void updateTweens(double dt);

extern void tween(double startValue, double endValue, double duration, std::function<void(double, double)> callback, Easing ease = linear);

//range
using Range = std::pair<double, double>;

extern const Range defaultRange;
extern double randomFromRange(const Range& range);

//particleemiter——emit function' name is changed to Emit
class ParticleEmitter;
extern std::vector<ParticleEmitter*> particleEmitters;
struct Particle {
  double x;
  double y;
  double vx;
  double vy;
  double bounce;
  double elapsed;
  double duration;
  int variant;
  double mass;
  double friction;
};

class ParticleEmitter {
private:
  static std::vector<Particle> pool;

public:
  std::set<Particle> particles;

  double x = 0;
  double y = 0;
  double w = 0;
  double h = 0;

  std::vector<std::vector<Sprite>> variants;
  double frequency = 0;
  Range velocity = defaultRange;
  Range angle = defaultRange;
  Range duration = defaultRange;
  Range bounce = defaultRange;
  Range friction = defaultRange;
  Range mass = defaultRange;

  double clock = 0;
  bool done = false;

  ParticleEmitter() {
    particleEmitters.push_back(this);
  }

  ParticleEmitter(const ParticleEmitter& other) = default;
  ParticleEmitter& operator=(const ParticleEmitter& other) = default;

  ParticleEmitter(ParticleEmitter&& other) noexcept = default;
  ParticleEmitter& operator=(ParticleEmitter&& other) noexcept = default;

  void extend(const ParticleEmitter& other);

  void remove();

  void update(double dt);

  void Emit();

  void burst(int count);
};


extern void updateParticles(double dt);


#endif // ENGINE_H
