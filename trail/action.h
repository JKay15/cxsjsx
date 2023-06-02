#ifndef ACTIONS_H
#define ACTIONS_H

#include "game.h"
#include "helpers.h"


void Damage(GameObject *object, int amount, GameObject *dealer = nullptr);
void Die(GameObject *object, GameObject *killer = nullptr);
void Cast();
void Resurrect();

#endif
