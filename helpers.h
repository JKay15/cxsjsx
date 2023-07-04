#ifndef HELPERS_H
#define HELPERS_H

#include <cmath>
#include <vector>

const double DEG_180 = M_PI;
const double DEG_90 = DEG_180 / 2;
const double DEG_270 = DEG_180 + DEG_90;
const double DEG_360 = DEG_180 * 2;

struct Point {
    double x;
    double y;
    Point(double x1=0,double x2=0):x(x1),y(x2){}
};

typedef std::vector<double> Vector;

// 创建一个点
Point createPoint(double x, double y);

struct Rectangle {
    double x;
    double y;
    double w;
    double h;
    Rectangle(double x1=0,double y1=0,double w1=0,double h1=0):x(x1),y(y1),w(w1),h(h1){}
};

// 创建一个矩形
Rectangle createRectangle(double x, double y, double w, double h);

// 判断两个矩形是否重叠
bool overlaps(Rectangle a, Rectangle b);

// 将值限制在给定的范围内
double clamp(double val, double min, double max);

// 根据给定的角度，创建一个向量
Vector vectorFromAngle(double radians);

// 计算两个点之间的夹角
double angleBetweenPoints(Point p1, Point p2);

// 从数组中删除指定的元素
template<typename T>
void removeFromArray(std::vector<T>& array, T element);

// 从数组中随机选择一个元素
template<typename T>
T randomElement(std::vector<T>& items);

// 计算两个点之间的距离
double distance(Point p1, Point p2);

// 根据给定的 X 和 Y 值，计算向量的角度
double vectorToAngle(double x, double y);

// 生成一个指定范围内的随机整数
int randomInt(int max);

// 生成一个指定范围内的随机浮点数
double randomFloat(double max = 1);

// 线程休眠指定时间
void sleep(int ms);

// 将数组中的元素随机排序
template<typename T>
std::vector<T> shuffled(std::vector<T> array);

#endif /* HELPERS_H */
