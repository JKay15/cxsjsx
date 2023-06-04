#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "basic.h"
#include <QDebug>
#include <QMouseEvent>
#include "basic.h"
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include "engine.h"

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        qDebug() << "click at " << event->pos(); // 打印点击位置的坐标
        QPointF pos = ui->graphicsView->mapToScene(event->pos());
        shoot(double(pos.x()),double(pos.y()));
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    // 初始化游戏界面
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);
    ui->graphicsView->resize(600, 400);

    //添加玩家
    player = new Player(100, 100, 50);
    QGraphicsPixmapItem *item_player = new QGraphicsPixmapItem();
    QPixmap playerSprite(":/images/player.png");
    QRect playerRect(0, 0, 50, 50); // 将 QRectF 对象转换为 QRect 对象
    QPixmap playerImage = playerSprite.copy(playerRect);

    item_player->setPixmap(playerImage);
    item_player->setPos(player->posx - item_player->boundingRect().width() / 2,
                        player->posy - item_player->boundingRect().height() / 2);
    scene->addItem(item_player);

    // 玩家获得子弹
    // 添加子弹
    Bullet* bullet = new Bullet(player->posx, player->posy - 20, 1, 0);
    player->bullets = bullet;
    item_bullet = new QGraphicsRectItem(0, 0, 10, 5);
    item_bullet->setBrush(Qt::black);
    item_bullet->setPos(bullet->posx - item_bullet->rect().width()/2.0, bullet->posy - item_bullet->rect().height()/2.0);
    scene->addItem(item_bullet);



    // 添加敌人
    enemy = new Enemy(220, 100, 50, 20, SPEED_ENEMY, 10, 5);
    item_enemy = new QGraphicsRectItem(0, 0, 50, 20);
    item_enemy->setBrush(Qt::red);
    item_enemy->setPos(enemy->posx - item_enemy->rect().width()/2.0, enemy->posy - item_enemy->rect().height()/2.0);
    scene->addItem(item_enemy);
    // mainwindow.cpp

    //添加定时器，每50ms执行一次
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(movePlayer()));
    connect(timer, SIGNAL(timeout()), this, SLOT(moveEnemy()));
    //connect(timer, SIGNAL(timeout()), this, SLOT(shoot()));
    timer->start(50);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::movePlayer()
{
    // 玩家移动
    if (player->posx >= scene->width() - item_player->rect().width()/2.0 && SPEED_PLAYER > 0) {
        return;
    }
    if (player->posx <= item_player->rect().width()/2.0 && SPEED_PLAYER < 0) {
        return;
    }

    player->move(0);
    item_player->setPos(player->posx - item_player->rect().width()/2.0, player->posy - item_player->rect().height()/2.0);

    // 更新子弹位置
    if (player->bullets) {
        Bullet* bullet = player->bullets;
        bullet->posx = player->posx;
        bullet->posy = player->posy - 20;
        item_bullet->setPos(bullet->posx - item_bullet->rect().width()/2.0, bullet->posy - item_bullet->rect().height()/2.0);
    }
}


void MainWindow::moveEnemy()
{
    // 敌人移动
    if (enemy->posx >= 400 && SPEED_ENEMY > 0) {
        SPEED_ENEMY = -SPEED_ENEMY;
    }
    if (enemy->posx <= 200 && SPEED_ENEMY < 0) {
        SPEED_ENEMY = -SPEED_ENEMY;;
    }


    enemy->move(SPEED_ENEMY);

    item_enemy->setPos(enemy->posx - item_enemy->rect().width()/2.0, enemy->posy - item_enemy->rect().height()/2.0);
}

void MainWindow::drawBulletTrajectory(Bullet* bullet)
{
    // Calculate the position of the bullet after one time step
    double nextX = bullet->posx + bullet->speed * bullet->angle_y;
    double nextY = bullet->posy + bullet->speed * bullet->angle_x;

    // Create a new QGraphicsLineItem to represent the bullet's trajectory
    QGraphicsLineItem* trajectoryLine = new QGraphicsLineItem(bullet->posx, bullet->posy, nextX, nextY);

    // Set the line's color and width
    QPen pen(Qt::black);
    pen.setWidth(2);
    trajectoryLine->setPen(pen);

    // Add the line to the scene
    scene->addItem(trajectoryLine);
}



void MainWindow::shoot(double mousex,double mousey)
{
    if (!player->bullets) {
        return;
    }

    Bullet* bullet = player->bullets;
    bullet->angle_x= calcAngleSin(Pos(player->posx,player->posy),Pos(mousex,mousey)).posx;
    bullet->angle_y= calcAngleSin(Pos(player->posx,player->posy),Pos(mousex,mousey)).posy;
    bullet->move(1);
    item_bullet->setPos(bullet->posx - item_bullet->rect().width()/2.0, bullet->posy - item_bullet->rect().height()/2.0);
    int num=0;
    // Call drawBulletTrajectory() function here
    while(num++<300){

    drawBulletTrajectory(bullet);
    bullet->posx =  bullet->posx + bullet->speed * bullet->angle_y;
    bullet->posy = bullet->posy + abs(double(num)/100 * bullet->angle_x);

    item_bullet->setPos(bullet->posx - item_bullet->rect().width()/2.0, bullet->posy - item_bullet->rect().height()/2.0);

    if (item_bullet->collidesWithItem(item_enemy))
    {
        qDebug()<<"fucked up";
        return;
    }
    else if (_outofrange(Pos(bullet->posx, bullet->posy),60,40))
    {
        //delete item_bullet;
        //return;
    }
    }
}
