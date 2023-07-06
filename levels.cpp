// levels.cpp
#include "levels.h"
#include "tags.h"
#include "objects.h"
#include "helpers.h"

// 查找表，用于获取生成函数
const std::vector<Spawn> LOOKUP = {
    Villager,
    Archer,
    Monk,
    Champion,
    Piper,
    RageKnight,
    RoyalGuard,
    ShellKnight,
    Wizard,
    TheKing,
    Rat,
    Villager,
    Bandit,
};

// 关卡数据
int LEVELS[] = {
    /// Level 1
    4, VILLAGER, END_OF_WAVE,
    4, VILLAGER, END_OF_WAVE,
    2, VILLAGER, 1, ARCHER, END_OF_WAVE,
    2, VILLAGER, 1, ARCHER, 4, VILLAGER, END_OF_LEVEL,
  
    // Level 2
    2, ARCHER, 4, VILLAGER, END_OF_WAVE,
    3, ARCHER, 4, VILLAGER, END_OF_WAVE,
    8, VILLAGER, 2, ARCHER, END_OF_WAVE,
    1, CHAMPION, END_OF_LEVEL,
  
    // Level 3
    1, MONK, END_OF_WAVE,
    4, BANDIT, END_OF_WAVE,
    2, BANDIT, 1, MONK, END_OF_WAVE,
    2, ARCHER, 1, MONK, END_OF_WAVE,
    4, VILLAGER, 2, BANDIT, 2, ARCHER, 1, MONK, END_OF_LEVEL,
  
    // Level 4
    1, SHELL_KNIGHT, END_OF_WAVE,
    4, VILLAGER, 3, BANDIT, END_OF_WAVE,
    1, SHELL_KNIGHT, 1, MONK, 1, END_OF_WAVE,
    2, ARCHER, 1, MONK, 1, SHELL_KNIGHT, END_OF_WAVE,
    8, VILLAGER, END_OF_WAVE,
    1, SHELL_KNIGHT, 1, CHAMPION, 1, SHELL_KNIGHT, END_OF_LEVEL,
  
    // Level 5 - Pied Piper (Miniboss)
    1, RAT, END_OF_WAVE,
    3, RAT, END_OF_WAVE,
    7, RAT, 1, PIPER, END_OF_LEVEL,
  
    // Level 6
    4, BANDIT, END_OF_WAVE,
    1, RAGE_KNIGHT, END_OF_WAVE,
    4, BANDIT, 1, CHAMPION, 2, ARCHER, END_OF_WAVE,
    4, BANDIT, 1, RAGE_KNIGHT, END_OF_WAVE,
    2, RAGE_KNIGHT, 1, MONK, END_OF_WAVE,
    1, WIZARD, END_OF_LEVEL,
  
    // Level 7 - Angry Mob
    20, MOB, 1, RAGE_KNIGHT, 20, MOB, 1, RAGE_KNIGHT, 20, MOB, END_OF_WAVE,
    20, MOB, 1, RAGE_KNIGHT, 20, MOB, 1, RAGE_KNIGHT, 20, MOB, END_OF_WAVE,
    3, CHAMPION, END_OF_LEVEL,
  
    // Level 8
    10, BANDIT, 1, MONK, 10, BANDIT, 1, MONK, END_OF_WAVE,
    10, BANDIT, 1, WIZARD, 1, SHELL_KNIGHT, END_OF_WAVE,
    5, BANDIT, 3, ARCHER, 3, RAGE_KNIGHT, END_OF_WAVE,
    1, CHAMPION, 1, WIZARD, 1, CHAMPION, END_OF_LEVEL,
  
    // Level 9 - Guards Approaching
    1, VILLAGER, END_OF_WAVE,
    2, ROYAL_GUARD, END_OF_WAVE,
    2, ARCHER, END_OF_WAVE,
    10, ROYAL_GUARD, END_OF_WAVE,
    10, ROYAL_GUARD, 2, MONK, 10, ROYAL_GUARD, END_OF_WAVE,
    2, ROYAL_GUARD, 1, SHELL_KNIGHT, 1, CHAMPION, 1, MONK, END_OF_WAVE,
    2, ROYAL_GUARD, 1, SHELL_KNIGHT, 1, CHAMPION, 1, WIZARD, END_OF_LEVEL,
  
    // Level 10 - The King (Boss Fight)
    1, THE_KING, END_OF_LEVEL,
};

std::unordered_map<int, std::function<int()>> DELAYS = {
    {RAT, []() { return randomInt(500); }},
    {VILLAGER, []() { return randomInt(200); }},
    {BANDIT, []() { return randomInt(200); }},
};

static double timer = 0;
static size_t cursor = 0;

bool isLevelFinished() {
    return LEVELS[cursor] == END_OF_LEVEL && isCleared();
}

bool isComplete() {
    return cursor >= sizeof(LEVELS) - 1;
}

void nextLevel() {
    cursor++;
    game.level++;
    game.onLevelStart();
}

void updateLevel(double dt) {
    int cmd = LEVELS[cursor];
    if ((timer -= dt) > 0) {}
    else if (cmd == END_OF_WAVE) isCleared() && cursor++;
    else if (cmd == END_OF_LEVEL) {}
    else if (cmd) {
        LEVELS[cursor]--; // Decrement quantity
        int id = LEVELS[cursor + 1];
        GameObject unit = LOOKUP[id]();
        game.spawn(unit);
        auto it = DELAYS.find(id);
        timer = unit.updateSpeed + (it != DELAYS.end() ? it->second() : 0);
    } else {
        cursor += 2;
    }
}
