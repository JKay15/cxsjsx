#include "basic.h"



// Pos 类的实现
Pos::Pos(double a, double b) : posx(a), posy(b) {}

Pos calcAngleSin(Pos p1, Pos p2)
{
    double dx = p2.posx - p1.posx;
    double dy = p2.posy - p1.posy;
    double dist = std::sqrt(dx * dx + dy * dy);
    if (dist == 0) {
        return Pos(0, 0); // 如果两个点重合，返回 0
    }

    double sinVal = dy / dist; // 计算 sin 值
    double cosVal = dx / dist;
    Pos ans(sinVal, cosVal);
    return ans;
}

// Bullet 类的实现
Bullet::Bullet(double x, double y, double sp, double a, double ax, double ay)
    : posx(x), posy(y), speed(sp), angle_x(ax), angle_y(ay), atk(a) {}

void Bullet::move(int time)
{
    posx += speed * angle_x * time;
    posy -= (0.5 * G * time * time + speed * angle_y * time);
}

// Player 类的实现
Player::Player(int h, double x, double y) : hp(h), posx(x), posy(y), bullets(nullptr) {}

void Player::move(double k)
{
    posx += k;
}

void Player::shoot(Pos mouse)
{
    Pos angle = calcAngleSin(Pos(posx, posy), mouse);
    bullets = new Bullet(posx, posy, 1, 10, angle.posx, angle.posy);

}

Pos Player::angle(Pos mouse)
{
    Pos temp(posx, posy);
    return calcAngleSin(temp, mouse);
}

void Player::die()
{
    //游戏结束
}

// Enemy 类的实现
Enemy::Enemy(int h, double x, double y, double a, double s, double rx, double ry)
    : hp(h), posx(x), posy(y), atk(a), speed(s), range_x(rx), range_y(ry) {}

void Enemy::move(double k)
{
    posx += k * speed;
}

void Enemy::die()
{

}

bool _outofrange( Pos pos, double range_x = 600, double range_y = 400)
{
    if (pos.posx < 0 || pos.posx > range_x || pos.posy < 0 || pos.posy > range_y) {
        return true;
    }
    return false;
}


void attack(Enemy* enemy, Bullet* bullet)
{
    if (bullet->posx < enemy->posx + enemy->range_x && bullet->posx > enemy->posx - enemy->range_x
        && bullet->posy < enemy->posy + enemy->range_y && bullet->posy > enemy->posy - enemy->range_y)
    {
        enemy->hp -= bullet->atk;
        if (enemy->hp <= 0)
        {
            enemy->die();
        }
        delete bullet;
    }
    else if (_outofrange(Pos(bullet->posx, bullet->posy),600,400))
    {
        delete bullet;
    }
}

void attack(Player* player, Enemy* Enemy)
{
    player->hp -= Enemy->atk;
    if (player->hp <= 0)
        player->die();
}
