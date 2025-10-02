//
// Created by 吴栋 on 2025/10/2.
//

#include "../WaveProcessor.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <utility>

#include "../struct/Point.h"

// 线性插值函数
double lerp(double a, double b, double t) {
    return a + t * (b - a);
}

// 寻找交叉区域
std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>> findOverlapRegion(
    const std::vector<Point>& wave1,
    const std::vector<Point>& wave2,
    double threshold = 0.1) {

    // 寻找wave1下降段的起始点
    size_t fallStart1 = wave1.size() - 1;
    for (size_t i = wave1.size() - 2; i > 0; --i) {
        if (wave1[i].y > wave1[i + 1].y) {
            fallStart1 = i;
            break;
        }
    }

    // 寻找wave2上升段的结束点
    size_t riseEnd2 = 0;
    for (size_t i = 1; i < wave2.size(); ++i) {
        if (wave2[i].y < wave2[i - 1].y) {
            riseEnd2 = i - 1;
            break;
        }
    }

    // 计算交叉区域
    double startX = std::max(wave1[fallStart1].x, wave2[0].x);
    double endX = std::min(wave1.back().x, wave2[riseEnd2].x);

    // 寻找wave1中交叉区域的起点
    size_t startIdx1 = fallStart1;
    while (startIdx1 < wave1.size() && wave1[startIdx1].x < startX) {
        startIdx1++;
    }

    // 寻找wave2中交叉区域的起点
    size_t startIdx2 = 0;
    while (startIdx2 < wave2.size() && wave2[startIdx2].x < startX) {
        startIdx2++;
    }

    // 寻找wave1中交叉区域的终点
    size_t endIdx1 = startIdx1;
    while (endIdx1 < wave1.size() && wave1[endIdx1].x < endX) {
        endIdx1++;
    }

    // 寻找wave2中交叉区域的终点
    size_t endIdx2 = startIdx2;
    while (endIdx2 < wave2.size() && wave2[endIdx2].x < endX) {
        endIdx2++;
    }

    // 修复1: 显式构造pair对象
    return std::make_pair(
        std::make_pair(startIdx1, endIdx1),
        std::make_pair(startIdx2, endIdx2)
    );
}

// 波形平滑融合函数
std::pair<std::vector<Point>, std::vector<Point>> smoothFusion(
    const std::vector<Point>& wave1,
    const std::vector<Point>& wave2) {

    // 修复2: 正确解包嵌套pair
    auto overlap = findOverlapRegion(wave1, wave2);
    auto overlap1 = overlap.first;
    auto overlap2 = overlap.second;

    size_t startIdx1 = overlap1.first;
    size_t endIdx1 = overlap1.second;
    size_t startIdx2 = overlap2.first;
    size_t endIdx2 = overlap2.second;

    // 如果没有找到交叉区域，直接拼接
    if (startIdx1 >= endIdx1 || startIdx2 >= endIdx2) {
        std::vector<Point> fusedWave = wave1;
        fusedWave.insert(fusedWave.end(), wave2.begin(), wave2.end());

        // 找到中点分割
        size_t midPoint = wave1.size();
        return {
            std::vector<Point>(fusedWave.begin(), fusedWave.begin() + midPoint),
            std::vector<Point>(fusedWave.begin() + midPoint, fusedWave.end())
        };
    }

    // 2. 创建融合区域
    std::vector<Point> fusedRegion;

    // 确定融合区域的起点和终点
    double startX = std::max(wave1[startIdx1].x, wave2[startIdx2].x);
    double endX = std::min(wave1[endIdx1 - 1].x, wave2[endIdx2 - 1].x);

    // 生成融合点
    size_t numPoints = static_cast<size_t>((endX - startX) / 5.0) + 1;
    for (size_t i = 0; i < numPoints; ++i) {
        double x = startX + i * 5.0;
        if (x > endX) break;

        // 在wave1中找到最近的2个点
        Point p1a, p1b;
        for (size_t j = startIdx1; j < endIdx1; ++j) {
            if (wave1[j].x >= x) {
                p1b = wave1[j];
                p1a = (j > startIdx1) ? wave1[j - 1] : p1b;
                break;
            }
        }

        // 在wave2中找到最近的2个点
        Point p2a, p2b;
        for (size_t j = startIdx2; j < endIdx2; ++j) {
            if (wave2[j].x >= x) {
                p2b = wave2[j];
                p2a = (j > startIdx2) ? wave2[j - 1] : p2b;
                break;
            }
        }

         // 插值计算（带除零保护）
        double t1 = (std::fabs(p1b.x - p1a.x) < 1e-10) ? 0 : (x - p1a.x) / (p1b.x - p1a.x);
        double y1 = lerp(p1a.y, p1b.y, t1);

        double t2 = (std::fabs(p2b.x - p2a.x) < 1e-10) ? 0 : (x - p2a.x) / (p2b.x - p2a.x);
        double y2 = lerp(p2a.y, p2b.y, t2);

        // 计算权重（带除零保护）
        double weight;
        if (numPoints > 1) {
            double t = static_cast<double>(i) / (numPoints - 1);
            weight = 0.5 - 0.5 * std::cos(t * M_PI);
        } else {
            weight = 0.5; // 单点情况取中间值
        }

        // 融合y值
        double y = lerp(y1, y2, weight);

        fusedRegion.push_back({x, y});
    }

    // 3. 构建最终波形
    std::vector<Point> fusedWave;

    // wave1的前半部分
    for (size_t i = 0; i < startIdx1; ++i) {
        fusedWave.push_back(wave1[i]);
    }

    // 融合区域
    fusedWave.insert(fusedWave.end(), fusedRegion.begin(), fusedRegion.end());

    // wave2的后半部分
    for (size_t i = endIdx2; i < wave2.size(); ++i) {
        fusedWave.push_back(wave2[i]);
    }

    // 4. 分割成前后两半
    size_t splitPoint = wave1.size();
    return {
        std::vector<Point>(fusedWave.begin(), fusedWave.begin() + splitPoint),
        std::vector<Point>(fusedWave.begin() + splitPoint, fusedWave.end())
    };
}