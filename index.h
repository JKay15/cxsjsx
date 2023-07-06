// index.h
#ifndef MAIN_H
#define MAIN_H

#include <vector>
#include <string>
#include "sprites.h"
#include "engine.h"
#include "game.h"
#include "renderer.h"
#include "actions.h"
#include "helpers.h"
#include "objects.h"
#include "levels.h"
#include "rituals.h"
#include "shop.h"
#include "fx.h"
//#include "sounds.h"
#include "behaviours.h"

// 声明全局变量和常量
extern GameObject player;
extern Game game;
extern bool paused;

extern const int ARROW_UP;
extern const int ARROW_DOWN;
extern const int SPACE;
extern const int ENTER;
extern const int KEY_P;

extern std::vector<std::string> INTRO_DIALOGUE;
extern std::vector<std::string> OUTRO_DIALOGUE;


// 声明函数原型
void onpointerup();
void onpointermove(QPainter &painter,int clientX, int clientY);
void onkeydown(int key);
void update(double dt);
void onWin();
void updateDialogue(double dt);
void wierd_initial();

#endif
