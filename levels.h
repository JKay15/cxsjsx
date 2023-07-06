#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include <functional>
#include <unordered_map>
#include "game.h"

// Signals (positive signals are spawn counters)
const int END_OF_LEVEL = 99;
const int END_OF_WAVE = 98;

// Spawn IDs
const int VILLAGER = 0;
const int ARCHER = 1;
const int MONK = 2;
const int CHAMPION = 3;
const int PIPER = 4;
const int RAGE_KNIGHT = 5;
const int ROYAL_GUARD = 6;
const int SHELL_KNIGHT = 7;
const int WIZARD = 8;
const int THE_KING = 9;
const int RAT = 10;
const int MOB = 11;
const int BANDIT = 12;

typedef GameObject (*Spawn)();

extern const std::vector<Spawn> LOOKUP;


// 判断关卡是否完成
bool isLevelFinished();
// 判断游戏是否完成
bool isComplete();
// 进入下一关卡
void nextLevel();
// 更新关卡状态
void updateLevel(double dt);
// 判断场上是否还有活着的对象
static bool isCleared() ;
#endif
