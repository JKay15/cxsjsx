#ifndef ACTIONS_H
#define ACTIONS_H

#include <string>
#include "game.h"
#include "helpers.h"
#include "objects.h"
#include "tags.h"
#include <chrono>
#include <thread>
#include "sprites.h"
#include "fx.h"

namespace actions{
void Damage(GameObject* object, int amount, GameObject* dealer = nullptr); // 造成伤害函数
void Die(GameObject* object, GameObject* killer = nullptr); // 死亡函数
void Cast(); // 施法函数
void Resurrect(); // 复活函数
}

#endif // ACTIONS_H

