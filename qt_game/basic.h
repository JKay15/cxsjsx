#ifndef BASIC_H
#define BASIC_H

#include <iostream>
#include <cstring>
#include <cmath>
const double G = 10;
class Pos
{
public:
    double posx, posy;
    Pos(double a, double b);

};

Pos calcAngleSin(Pos p1, Pos p2);

class Bullet
{
public:
    double posx, posy;
    double speed;
    double angle_x, angle_y;
    double atk;
    Bullet* next;
    // 添加构造函数
    Bullet(double x, double y, double sp, double a, double ax = 0, double ay = 0);

    void move(int time);
};

class Player
{
public:
    int hp;
    double posx, posy;
    Bullet* bullets; // 存放子弹

    // 添加构造函数
    Player(int h, double x, double y);

    void move(double k);

    void shoot(Pos mouse);

    Pos angle(Pos mouse);

    void die();
};

class Enemy
{
public:
    int hp;
    double posx, posy;
    double atk;
    double speed;
    double range_x, range_y; // 子弹击中判定范围

    // 添加构造函数
    Enemy(int h, double x, double y, double a, double s, double rx, double ry);

    void move(double k);

    void die();

};

bool _outofrange(Pos bullet,double x,double y);

void attack(Enemy* enemy, Bullet* bullet);

void attack(Player* player, Enemy* Enemy);

#endif // BASIC_H
