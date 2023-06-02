#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

const double DEG_180 = M_PI;
const double DEG_90 = DEG_180 / 2;
const double DEG_270 = DEG_180 + DEG_90;
const double DEG_360 = DEG_180 * 2;

struct Point {
    double x;
    double y;
};

typedef vector<double> Vector;

Point createPoint(double x, double y) {
    Point p;
    p.x = x;
    p.y = y;
    return p;
}

struct Rectangle {
    double x;
    double y;
    double w;
    double h;
};

Rectangle createRectangle(double x, double y, double w, double h) {
    Rectangle r;
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    return r;
}

bool overlaps(Rectangle a, Rectangle b) {
    return (a.x < b.x + b.w && a.y < b.y + b.h &&
            a.x + a.w > b.x && a.y + a.h > b.y);
}

double clamp(double val, double min, double max) {
    return val < min ? min : val > max ? max : val;
}

Vector vectorFromAngle(double radians) {
    Vector v;
    v.push_back(cos(radians));
    v.push_back(sin(radians));
    return v;
}

double angleBetweenPoints(Point p1, Point p2) {
    return atan2(p2.y - p1.y, p2.x - p1.x);
}

template<typename T>
void removeFromArray(vector<T>& array, T element) {
    typename vector<T>::iterator it = find(array.begin(), array.end(), element);
    if (it != array.end()) {
        array.erase(it);
    }
}

template<typename T>
T randomElement(vector<T>& items) {
    int index = rand() % items.size();
    return items[index];
}

double distance(Point p1, Point p2) {
    return hypot(p2.x - p1.x, p2.y - p1.y);
}

double vectorToAngle(double x, double y) {
    return atan2(y, x);
}

int randomInt(int max) {
    return rand() % max;
}

double randomFloat(double max = 1) {
    return static_cast<double>(rand()) / RAND_MAX * max;
}

void sleep(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

template<typename T>
vector<T> shuffled(vector<T> array) {
    int m = array.size();

    while (m) {
        int i = randomInt(m--);
        swap(array[m], array[i]);
    }

    return array;
}

int main() {
    srand(time(NULL)); // 初始化随机数生成器

    // 调用函数示例
    Point p = createPoint(10, 20);
    Rectangle r = createRectangle(0, 0, 100, 50);
    bool isOverlap = overlaps(p, r);
    Vector v = vectorFromAngle(DEG_90);
    double angle = angleBetweenPoints(p, createPoint(30, 40));
    vector<int> arr = {1, 2, 3, 4, 5};
    removeFromArray(arr, 3);
    int randomNum = randomInt(10);
    double randomFloatNum = randomFloat();
    sleep(1000);

    return 0;
}
