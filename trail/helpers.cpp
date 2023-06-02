#include "helpers.h"
#include <algorithm>
#include <chrono>
#include <thread>

// 创建一个点
Point createPoint(double x, double y) {
    Point p;
    p.x = x;
    p.y = y;
    return p;
}

// 创建一个矩形
Rectangle createRectangle(double x, double y, double w, double h) {
    Rectangle r;
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    return r;
}

// 判断两个矩形是否重叠
bool overlaps(Rectangle a, Rectangle b) {
    return (a.x < b.x + b.w && a.y < b.y + b.h &&
            a.x + a.w > b.x && a.y + a.h > b.y);
}

// 将值限制在给定的范围内
double clamp(double val, double min, double max) {
    return val < min ? min : val > max ? max : val;
}

// 根据给定的角度，创建一个向量
Vector vectorFromAngle(double radians) {
    Vector v;
    v.push_back(cos(radians));
    v.push_back(sin(radians));
    return v;
}

// 计算两个点之间的夹角
double angleBetweenPoints(Point p1, Point p2) {
    return atan2(p2.y - p1.y, p2.x - p1.x);
}

// 从数组中删除指定的元素
template<typename T>
void removeFromArray(std::vector<T>& array, T element) {
    typename std::vector<T>::iterator it = find(array.begin(), array.end(), element);
    if (it != array.end()) {
        array.erase(it);
    }
}

// 从数组中随机选择一个元素
template<typename T>
T randomElement(std::vector<T>& items) {
    int index = rand() % items.size();
    return items[index];
}

// 计算两个点之间的距离
double distance(Point p1, Point p2) {
    return hypot(p2.x - p1.x, p2.y - p1.y);
}

// 根据给定的 X 和 Y 值，计算向量的角度
double vectorToAngle(double x, double y) {
    return atan2(y, x);
}

// 生成一个指定范围内的随机整数
int randomInt(int max) {
    return rand() % max;
}

// 生成一个指定范围内的随机浮点数
double randomFloat(double max) {
    return static_cast<double>(rand()) / RAND_MAX * max;
}

// 线程休眠指定时间
void sleep(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// 将数组中的元素随机排序
template<typename T>
std::vector<T> shuffled(std::vector<T> array) {
    int m = array.size();

    while (m) {
        int i = randomInt(m--);
        std::swap(array[m], array[i]);
    }

    return array;
}
