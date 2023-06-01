#include "index.h"
#include <QMouseEvent>
#include <QKeyEvent>

GameView::GameView(QGraphicsScene *scene) : QGraphicsView(scene) {}

void GameView::mouseReleaseEvent(QMouseEvent *event)
{
    // 处理鼠标点击事件
    if (gameState == INTRO) {
        // 播放音乐并将游戏状态更改为游戏中
        // 调用Cast函数
    }
}

void GameView::mouseMoveEvent(QMouseEvent *event)
{
    // 处理鼠标移动事件
    // 根据鼠标位置更新游戏中的法术目标角度
}

void GameView::keyPressEvent(QKeyEvent *event)
{
    // 处理键盘事件
    if (gameState == PLAYING) {
        if (event->key() == Qt::Key_Space) {
            // 调用Resurrect函数
        }
    } else if (gameState == SHOPPING) {
        if (event->key() == Qt::Key_Up) {
            // 调用selectShopIndex函数
        } else if (event->key() == Qt::Key_Down) {
            // 调用selectShopIndex函数
        } else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
            // 调用buy函数
        }
    }
}
