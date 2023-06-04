#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include "basic.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;



    QGraphicsScene* scene;

    Player* player;
    Enemy* enemy;
    QTimer* timer;
    QGraphicsRectItem* item_player;
    QGraphicsRectItem* item_enemy;
    QGraphicsRectItem* item_bullet;

    void drawBulletTrajectory(Bullet* bullet);
    void mousePressEvent(QMouseEvent *event);
    double SPEED_PLAYER = 5;
    double SPEED_ENEMY = 2;
    // mainwindow.h
 public slots:
    void shoot(double mousex,double mousey);

private slots:
    void movePlayer();
    void moveEnemy();

};




#endif // MAINWINDOW_H
