//
// Created by 吴栋 on 2025/10/2.
//

#include "WaveProcessor.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <utility>

#include "../struct/Point.h"

// 线性插值函数
double lerpw(double a, double b, double t) {
    return a + t * (b - a);
}

std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>> findOverlapRegion(
    const std::vector<Point>& wave1,
    const std::vector<Point>& wave2,
    double minSlopeThreshold = 0.01, // 最小斜率阈值
    int windowSize = 5) {           // 滑动窗口大小

    // 1. 寻找wave1下降段的起始点（使用滑动窗口平均）
    size_t fallStart1 = wave1.size() - 1;
    for (size_t i = windowSize; i < wave1.size() - windowSize; ++i) {
        double leftAvg = 0.0;
        double rightAvg = 0.0;

        // 计算左侧窗口平均值
        for (int j = -windowSize; j < 0; ++j) {
            leftAvg += wave1[i + j].y;
        }
        leftAvg /= windowSize;

        // 计算右侧窗口平均值
        for (int j = 1; j <= windowSize; ++j) {
            rightAvg += wave1[i + j].y;
        }
        rightAvg /= windowSize;

        // 计算斜率（使用窗口平均值）
        double slope = (rightAvg - leftAvg) / (wave1[i + windowSize].x - wave1[i - windowSize].x);

        // 检测下降趋势
        if (slope < -minSlopeThreshold) {
            fallStart1 = i;
            break;
        }
    }

    // 2. 寻找wave2上升段的结束点（使用滑动窗口平均）
    size_t riseEnd2 = 0;
    for (size_t i = windowSize; i < wave2.size() - windowSize; ++i) {
        double leftAvg = 0.0;
        double rightAvg = 0.0;

        // 计算左侧窗口平均值
        for (int j = -windowSize; j < 0; ++j) {
            leftAvg += wave2[i + j].y;
        }
        leftAvg /= windowSize;

        // 计算右侧窗口平均值
        for (int j = 1; j <= windowSize; ++j) {
            rightAvg += wave2[i + j].y;
        }
        rightAvg /= windowSize;

        // 计算斜率（使用窗口平均值）
        double slope = (rightAvg - leftAvg) / (wave2[i + windowSize].x - wave2[i - windowSize].x);

        // 检测上升趋势结束（开始下降）
        if (slope < minSlopeThreshold) {
            riseEnd2 = i;
            break;
        }
    }

    // 3. 计算交叉区域
    double startX = std::max(wave1[fallStart1].x, wave2[0].x);
    double endX = std::min(wave1.back().x, wave2[riseEnd2].x);

    // 4. 寻找wave1中交叉区域的起点
    size_t startIdx1 = fallStart1;
    while (startIdx1 < wave1.size() && wave1[startIdx1].x < startX) {
        startIdx1++;
    }

    // 5. 寻找wave2中交叉区域的起点
    size_t startIdx2 = 0;
    while (startIdx2 < wave2.size() && wave2[startIdx2].x < startX) {
        startIdx2++;
    }

    // 6. 寻找wave1中交叉区域的终点
    size_t endIdx1 = startIdx1;
    while (endIdx1 < wave1.size() && wave1[endIdx1].x < endX) {
        endIdx1++;
    }

    // 7. 寻找wave2中交叉区域的终点
    size_t endIdx2 = startIdx2;
    while (endIdx2 < wave2.size() && wave2[endIdx2].x < endX) {
        endIdx2++;
    }

    // 确保索引有效
    startIdx1 = std::min(startIdx1, wave1.size() - 1);
    endIdx1 = std::min(endIdx1, wave1.size() - 1);
    startIdx2 = std::min(startIdx2, wave2.size() - 1);
    endIdx2 = std::min(endIdx2, wave2.size() - 1);

    return {
        {startIdx1, endIdx1},
        {startIdx2, endIdx2}
    };
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

    std::cout << "size " << wave1.size() << " " << wave2.size() << std::endl;
    std::cout << "overlap: " << startIdx1 << " " << endIdx1 << " " << startIdx2 << " " << endIdx2 << std::endl ;
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
        double y1 = lerpw(p1a.y, p1b.y, t1);

        double t2 = (std::fabs(p2b.x - p2a.x) < 1e-10) ? 0 : (x - p2a.x) / (p2b.x - p2a.x);
        double y2 = lerpw(p2a.y, p2b.y, t2);

        // 计算权重（带除零保护）
        double weight;
        if (numPoints > 1) {
            double t = static_cast<double>(i) / (numPoints - 1);
            weight = 0.5 - 0.5 * std::cos(t * M_PI);
        } else {
            weight = 0.5; // 单点情况取中间值
        }

        // 融合y值
        double y = lerpw(y1, y2, weight);

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