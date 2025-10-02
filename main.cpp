#include <iostream>
#include "struct/Point.h"
#include "generator/WaveGen.h"
#include "processor/WaveProcessor.h"

int main() {

    auto wave1 = generateMountainWave(
        0, 100,   // X范围
        30, 70,   // 山峰位置
        0,       // 起点高度
        50,       // 山峰高度相对于起点的增量 (10+40=50)
        20,       // 终点高度
        0,        // 平缓波形
        0.0,      // 凹陷/凸起强度
        0.0,      // 凹陷/凸起宽度
        0.5,      // 凹陷/凸起位置
        true,     // 允许负值
        true      // 平滑基础曲线
    );

    auto wave2 = generateMountainWave(
        80, 180,   // X范围
        110, 150,   // 山峰位置
        0,       // 起点高度
        50,       // 山峰高度相对于起点的增量 (10+40=50)
        20,       // 终点高度
        0,        // 平缓波形
        0.0,      // 凹陷/凸起强度
        0.0,      // 凹陷/凸起宽度
        0.5,      // 凹陷/凸起位置
        true,
        true
    );

    auto [firstHalf,secondHalf] = smoothFusion(wave1, wave2);

    // 输出波形1的点
    std::cout << "Wave 1 Points (Smoothed Base):\n";
    for (const auto& p : wave1) {
        std::cout << "(" << p.x << ", " << p.y << ")\n";
    }

    std::cout << "Wave 2 Points (Smoothed Base):\n";
    for (const auto& p : wave2) {
        std::cout << "(" << p.x << ", " << p.y << ")\n";
    }

    std::cout << "FWave 1 Points (Smoothed Base):\n";
    for (const auto& p : firstHalf) {
        std::cout << "(" << p.x << ", " << p.y << ")\n";
    }

    std::cout << "FWave 2 Points (Smoothed Base):\n";
    for (const auto& p : secondHalf) {
        std::cout << "(" << p.x << ", " << p.y << ")\n";
    }



}