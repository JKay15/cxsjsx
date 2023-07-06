// engine.cpp
#include "engine.h"
#include "helpers.h"
#include <set>
#include <utility>
#include <QPixmap>
#include "helpers.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <QRect>
#include <QJsonObject>
#include <QFile>
#include <QJsonDocument>
#include <QApplication>
#include <QPainter>
#include <QGuiApplication>
#include <QScreen>
#include <chrono>
#include <functional>
#include <QCoreApplication>
#include <QDateTime>
#include <QImage>
#include <QPainter>
#include <QResizeEvent>
#include <QWidget>
#include <QTimer>
#include <random>


// Global variables
QImage canvas;
//QPixmap spriteSrc("./sprites.png");
//QImage spritesImage = spriteSrc.toImage();
QPixmap *spriteSrc = nullptr;
QImage spritesImage;
int glyphWidth;
int glyphHeight;
int lineHeight;
QJsonObject glyphWidthOverrides;
std::unordered_map<char, int> metrics;
int textX = 0;
int textY = 0;
std::vector<Tween> tweens;
const Range defaultRange={0,0};

std::vector<Particle> ParticleEmitter::pool;
std::vector<ParticleEmitter*> particleEmitters;

//font import
void import_font(){
    // 读取 JSON 文件
    QFile file("./font.json");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // 处理文件打开错误
    }

    // 解析 JSON 数据
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    file.close();

    // 获取根对象
    QJsonObject rootObject = jsonDoc.object();

    // 获取 glyphWidth
    glyphWidth = rootObject.value("glyphWidth").toInt();

    // 获取 glyphHeight
    glyphHeight = rootObject.value("glyphHeight").toInt();

    // 获取 lineHeight
    lineHeight = rootObject.value("lineHeight").toInt();

    // 获取 glyphWidthOverrides
    glyphWidthOverrides = rootObject.value("glyphWidthOverrides").toObject();

    // 将 glyphWidthOverrides 存入 std::unordered_map<char, int> metrics
    for(const QString& key : glyphWidthOverrides.keys()) {
        metrics[key.at(0).toLatin1()] = glyphWidthOverrides.value(key).toInt();
    }
}

// Canvas methods

void clear() {
    // 清空画布
    canvas.fill(Qt::transparent);
}

void drawSpriteSlice(int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh) {
    // 使用 QPainter 进行绘制
    QPainter painter(&canvas);
    painter.drawImage(QRect(dx, dy, dw, dh), spritesImage, QRect(sx, sy, sw, sh));
    painter.end();
}
void drawSprite(const Sprite& sprite, int x, int y) {
    // Implementation for drawing a sprite at the specified position
     drawSpriteSlice(sprite[0], sprite[1], sprite[2], sprite[3], x, y, sprite[2], sprite[3]);
}

void drawSceneSprite(const Sprite& sprite, int x, int y) {
    // Implementation for drawing a scene sprite at the specified position
    drawSprite(sprite, x, -y - sprite[3]);
}

void drawNineSlice(const Sprite& sprite, int x, int y, int w, int h) {
    // Implementation for drawing a nine-slice sprite at the specified position and dimensions
    int sx = sprite[0];
    int sy = sprite[1];
    int sw = sprite[2];
    int sh = sprite[3];
    int c = 3;
    if (w <= c || h <= c) return;
    int sx1 = sx;
    int sx2 = sx + c;
    int sx3 = sx + sw - c;
    int sy1 = sy;
    int sy2 = sy + c;
    int sy3 = sy + sh - c;
    int sw1 = sx3 - sx2;
    int sh1 = sy3 - sy2;
    int dx1 = x;
    int dx2 = x + c;
    int dx3 = x + w - c;
    int dy1 = y;
    int dy2 = y + c;
    int dy3 = y + h - c;
    int dw1 = dx3 - dx2;
    int dh1 = dy3 - dy2;
    drawSpriteSlice(sx1, sy1, c, c, dx1, dy1, c, c); // Top left corner
    drawSpriteSlice(sx3, sy1, c, c, dx3, dy1, c, c); // Top right corner
    drawSpriteSlice(sx1, sy3, c, c, dx1, dy3, c, c); // Bottom left corner
    drawSpriteSlice(sx3, sy3, c, c, dx3, dy3, c, c); // Bottom right corner
    drawSpriteSlice(sx2, sy1, sw1, c, dx2, dy1, dw1, c); // Top
    drawSpriteSlice(sx2, sy3, sw1, c, dx2, dy3, dw1, c); // Bottom
    drawSpriteSlice(sx1, sy2, c, sh1, dx1, dy2, c, dh1); // Left
    drawSpriteSlice(sx3, sy2, c, sh1, dx3, dy2, c, dh1); // Right
    drawSpriteSlice(sx2, sy2, sw1, sh1, dx2, dy2, dw1, dh1); // Center
}

// Write text
void write(const std::string& text, int x, int y) {
    // Implementation for writing text at the specified position
    textX = x;
    textY = y;

    QPainter painter(&canvas);

    for (char c : text) {
        if (c == '\n') {
            textX = x;
            textY += lineHeight;
        } else {
            int code = c - 32;
            int sx = (code % 32) * glyphWidth;
            int sy = (code / 32) * glyphHeight;
            int dx = textX;
            int dy = textY;
            painter.drawImage(dx, dy, spritesImage, sx, sy, glyphWidth, glyphHeight);
            textX += metrics.count(c) ? metrics[c] : glyphWidth;
        }
    }
}

// Resize
void resize() {
    // Implementation for resizing the canvas
    int w = canvas.width();
    int h = canvas.height();
    double scale = std::min(static_cast<double>(QGuiApplication::primaryScreen()->geometry().width()) / w,
                            static_cast<double>(QGuiApplication::primaryScreen()->geometry().height()) / h);
    int newWidth = static_cast<int>(w * scale);
    int newHeight = static_cast<int>(h * scale);
    // 调整画布大小
    canvas = canvas.scaled(newWidth, newHeight, Qt::IgnoreAspectRatio, Qt::FastTransformation);
}

// Init

void init(int width, int height, void (*update)(double)) {
    // 调整画布大小
    canvas = QImage(width, height, QImage::Format_ARGB32);
    resize();

    // 定义时间变量
    auto t0 = QDateTime::currentMSecsSinceEpoch();

    // 定时器
    QTimer* timer = new QTimer();
    QObject::connect(timer, &QTimer::timeout, [update, t0]() mutable {
        auto t1 = QDateTime::currentMSecsSinceEpoch();
        double dt = (t1 - t0) / 1000.0;
        update(dt);
        t0 = t1;
    });
    timer->start(16); // 每隔16毫秒更新一次，大约60帧每秒

    // 运行应用程序事件循环
    QCoreApplication::exec();
}

// Easing
double linear(double x) {
    // Implementation for the linear easing function
     return x;
}

void updateTweens(double dt) {
    // Implementation for updating the active tweens
    // 过滤未完成的 tweens
    tweens.erase(std::remove_if(tweens.begin(), tweens.end(), [&](Tween& tween) {
        tween.elapsed += dt;
        double progress = clamp(tween.elapsed / tween.duration, 0.0, 1.0);
        double t = tween.ease(progress);
        double value = tween.startValue + (tween.endValue - tween.startValue) * t;
        tween.callback(value, t);
        return progress < 1.0;
    }), tweens.end());
}

void tween(double startValue, double endValue, double duration, std::function<void(double, double)> callback, Easing ease) {
    // Implementation for creating a new tween
     tweens.push_back({ startValue, endValue, duration, 0, ease, callback });
}

// Range
double randomFromRange(const Range& range) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(range.first, range.second);
  return dis(gen);
}

// ParticleEmitter methods
ParticleEmitter& ParticleEmitter::extend(double Frequency,std::vector<std::vector<Sprite>> Variant,Range Mass,Range Velocity) {
    // Implementation for extending the current ParticleEmitter with properties from another ParticleEmitter
    // 将 other 的属性扩展到当前 ParticleEmitter 对象
    frequency=Frequency;
    if(Variant.size()!=0){
        variants=Variant;
    }
    if(Mass!=defaultRange){
        mass=Mass;
    }
    if(Velocity!=defaultRange){
        velocity=Velocity;
    }
}
ParticleEmitter& ParticleEmitter::extend(double X,double Y) {
    x=X;
    y=Y;
}
ParticleEmitter& ParticleEmitter::extend(Rectangle rec) {
    x=rec.x;
    y=rec.y;
    w=rec.w;
    h=rec.h;
}
ParticleEmitter& ParticleEmitter::extend(std::vector<std::vector<Sprite>> Variant) {
    variants=Variant;
}
ParticleEmitter& ParticleEmitter::extend(std::vector<std::vector<Sprite>> Variant,double Frequency,Range Angle,Range Mass) {
    // Implementation for extending the current ParticleEmitter with properties from another ParticleEmitter
    // 将 other 的属性扩展到当前 ParticleEmitter 对象
    frequency=Frequency;
    if(Variant.size()!=0){
        variants=Variant;
    }
    if(Mass!=defaultRange){
        mass=Mass;
    }
    if(Angle!=defaultRange){
        angle=Angle;
    }
}
ParticleEmitter& ParticleEmitter::extend(double X,double Y,std::vector<std::vector<Sprite>> Variant,Range Duration){
    x=X;
    y=Y;
    if(Variant.size()!=0){
        variants=Variant;
    }
    duration=Duration;
}
void ParticleEmitter::remove() {
    // Implementation for removing the ParticleEmitter
    done = true;
}

void ParticleEmitter::update(double dt) {
  double t = dt / 1000.0;

  clock += frequency;
  while (!done && clock > 0) {
    clock -= 1;
    Emit();
  }
  for (std::set<Particle>::iterator it = particles.begin(); it != particles.end();) {
    Particle& p = const_cast<Particle&>(*it);
    if ((p.elapsed += dt) >= p.duration) {
      it = particles.erase(it);
      pool.push_back(p);
    } else {
      p.x += p.vx * t;
      p.y += p.vy * t;
      p.vy -= p.mass * t;

      if (p.y <= 0) {
        p.y = 0;
        p.vy *= -p.bounce;
        p.vx *= p.friction;
      }

      ++it;
    }
  }
  if (done && particles.empty()) {
    auto it = std::find(particleEmitters.begin(), particleEmitters.end(), this);
    if (it != particleEmitters.end()) {
      particleEmitters.erase(it);
    }
  }
}
//实现角度和向量的转化
std::pair<double, double> vectorFromAngle1(double angle) {
    double radians = angle * M_PI / 180.0;
    double vx = std::cos(radians);
    double vy = std::sin(radians);
    return std::make_pair(vx, vy);
}
void ParticleEmitter::Emit() {
    Particle p;
    double velocity = randomFromRange(this->velocity);
    double angle = randomFromRange(this->angle);
    double vx, vy;
    std::pair<double, double> velocityComponents = vectorFromAngle1(angle);
    vx = velocityComponents.first;
    vy = velocityComponents.second;
    p.vx = vx * velocity;
    p.vy = vy * velocity;
    p.elapsed = 0;
    p.duration = randomFromRange(this->duration);
    p.bounce = randomFromRange(this->bounce);
    p.friction = randomFromRange(this->friction);
    p.mass = randomFromRange(this->mass);
    p.variant = static_cast<int>(randomFromRange({0, static_cast<double>(this->variants.size())}));

    particles.insert(p); // Insert the new particle into the set
}

ParticleEmitter& ParticleEmitter::burst(int count) {
    // Implementation for bursting particles
    for (int i = 0; i < count; i++) {
      Emit();
    }
}

// Other functions
void updateParticles(double dt) {
    // Implementation for updating all particle emitters
    for (ParticleEmitter* emitter : particleEmitters) {
      emitter->update(dt);
    }
}
