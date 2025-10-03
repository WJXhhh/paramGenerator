//
// Created by 吴栋 on 2025/10/2.
//

#ifndef PARAMGENERATOR_POINT_H
#define PARAMGENERATOR_POINT_H
#include <cmath>


struct Point {
    double x,y;

    bool operator==(const Point& other) const {
        return std::abs(x - other.x) < 1e-5 && std::abs(y - other.y) < 1e-5;
    }

    // 用于unordered_set的哈希函数
    struct Hash {
        size_t operator()(const Point& p) const {
            size_t h1 = std::hash<double>()(p.x);
            size_t h2 = std::hash<double>()(p.y);
            return h1 ^ (h2 << 1);
        }
    };
};


#endif //PARAMGENERATOR_POINT_H