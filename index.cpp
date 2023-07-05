// main.cpp
#include "index.h"

// 初始化全局变量和常量
Player player = Player();
Game game = Game(player);
bool paused = false;

const int ARROW_UP = 38;
const int ARROW_DOWN = 40;
const int SPACE = 32;
const int ENTER = 13;
const int KEY_P = 80;

std::vector<std::string> INTRO_DIALOGUE = {
  "Norman wasn't a particularly popular necromancer...",
  "         The other villagers hunted him.",
  "     Sometimes they even finished the job (@)",
  "  But like any self-respecting necromancer...",
  "        Norman just brought himself back.",
};

std::vector<std::string> OUTRO_DIALOGUE = {
  "",
  "It was over.",
  "Norman was able to study peacefully.",
  "But he knew that eventually, they'd be back.",
  "THE END",
};

// 当鼠标抬起时的事件处理函数
void onpointerup() {
    // 如果游戏处于介绍状态，就开始游戏，并改变玩家的精灵
    if (game.state == INTRO) {
        play();
        game.state = PLAYING;
        game.player.sprite = sprites.norman_arms_down;
    }

    // 发射法术
    Cast();
}

// 当鼠标移动时的事件处理函数
void onpointermove(int clientX, int clientY) {
    // 计算玩家和鼠标位置之间的角度，并设置法术的目标角度
    auto p1 = player.center();
    auto p2 = screenToSceneCoords(clientX, clientY);
    game.spell.targetAngle = angleBetweenPoints(p1, p2);
}

// 当按下键盘时的事件处理函数
void onkeydown(int key) {
    // 如果游戏处于进行状态，根据按键执行不同的操作
    if (game.state == PLAYING) {
        // 如果按下空格键，就复活一个敌人作为盟友
        if (key == SPACE) Resurrect();
        // 如果按下P键，就暂停或恢复游戏
        if (key == KEY_P) paused = !paused;
    }
    else if (game.state == SHOPPING) {
        // 如果游戏处于商店状态，根据按键选择或购买仪式
        if (key == ARROW_UP) selectShopIndex(-1);
        if (key == ARROW_DOWN) selectShopIndex(+1);
        if (key == ENTER) buy();
    }
}

bool normanIsBouncing = false;

// 更新游戏逻辑的函数，参数是时间间隔（毫秒）
void update(double dt) {
    // 更新对话框和渲染画面
    updateDialogue(dt);
    render(dt);

    // 如果游戏暂停，就不进行后续的更新
    if (paused) return;

    // 如果游戏处于进行状态，就更新关卡逻辑
    if (game.state == PLAYING) {
        updateLevel(dt);
    }

    // 如果游戏不是处于介绍状态，就更新游戏对象和法术逻辑
    if (game.state != INTRO) {
        game.update(dt);
    }

    // 更新缓动动画和粒子效果
    updateTweens(dt);
    updateParticles(dt);

    // 如果游戏处于进行状态并且关卡完成，就判断是否整个游戏胜利，或者进入商店
    if (game.state == PLAYING && isLevelFinished()) {
        if (isComplete()) {
            onWin();
        }
        else {
            game.onLevelEnd();
            enterShop();
        }
    }

    // 如果游戏进入第二关，并且玩家还没有弹跳的行为，就给玩家添加一个弹跳的行为
    if (game.level == 2 && !normanIsBouncing) {
        game.player.addBehaviour(new March(game.player, 0));
        game.player.updateClock = 100;
        game.player.updateSpeed = 60000 / BPM * 2;
        normanIsBouncing = true;
    }
}

// 当游戏胜利时的函数
void onWin() {
    // 设置游戏状态为胜利，并显示结束对话框
    game.state = WIN;
    game.dialogue = OUTRO_DIALOGUE;
}

double dialogueTimer = 0;

// 更新对话框的函数，参数是时间间隔（毫秒）
void updateDialogue(double dt) {
    // 如果对话框计时器超过4秒，就移除第一句对话，并重置计时器
    if ((dialogueTimer += dt) > 4000) {
        game.dialogue.shift()
            dialogueTimer = 0;

        // 如果玩家观看了整个介绍对话框，就提醒他们点击开始游戏
        if (game.state == INTRO && game.dialogue.length == 0) {
            game.dialogue.push("                (Click to begin)");
        }
    }
}

// 给游戏添加一个连击仪式
game.addRitual(Streak);

// 设置商店的可选仪式列表
shop.rituals = {
   Bouncing,
   Ceiling,
   Rain,
   Doubleshot,
   Hunter,
   Weightless,
   Knockback,
   Drunkard,
   Seer,
   Tearstone,
   Impatience,
   Bleed,
   Salvage,
   Studious,
   Electrodynamics,
   Chilly,
   Giants,
   Avarice,
   Hardened,
   Allegiance,
};

// 设置游戏的初始对话框为介绍对话框
game.dialogue = INTRO_DIALOGUE;

// 初始化游戏引擎，并传入画面宽度、高度和更新函数
init(game.stage.width, game.stage.height, update);
// 产生一些粒子效果
dust().burst(200);
