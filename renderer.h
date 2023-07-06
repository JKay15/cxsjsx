// renderer.h
#include <QPainter>
#include<sprites.h>
#include <engine.h>
#include <helpers.h>
#include <game.h>
#include <shop.h>
#include <behaviours.h>


    void screenshake(QPainter &painter, double time);
    Point screenToSceneCoords(QPainter &painter, double x, double y);
    void render(QPainter &painter, double dt);
    void drawShop(QPainter &painter);
    void drawHud(QPainter &painter);
    void drawOrbs(double x, double y, double value, double maxValue, const Sprite &sprite, const Sprite &emptySprite);
    void drawObjects(QPainter &painter);
    void drawBackground();
    void drawReticle();
    void drawParticles();

