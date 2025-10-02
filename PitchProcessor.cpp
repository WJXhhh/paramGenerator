//
// Created by 吴栋 on 2025/10/2.
//

#include "PitchProcessor.h"

#include <vector>
#include <cmath>

#include "Point.h"

double lerp(double a, double b, double t) {
    return a + t * (b - a);
}

// 计算偏移量
double calculateOffset(double pbs, int m, int n) {
    return (8192.0 / pbs) * (m - n);
}

// 应用偏移量到波形
void applyOffset(std::vector<Point>& wave, double offset) {
    for (auto& point : wave) {
        point.y += offset;
    }
}

// 寻找重叠区域
std::pair<size_t, size_t> findOverlapRegion(
    const std::vector<Point>& wave1,
    const std::vector<Point>& wave2,
    double threshold = 5.0) {

    // 检查是否有重叠
    if (wave1.empty() || wave2.empty() || wave1.back().x < wave2.front().x) {
        return {wave1.size(), 0}; // 没有重叠
    }

    // 寻找wave1中与wave2开始重叠的位置
    size_t startIdx1 = wave1.size() - 1;
    for (size_t i = 0; i < wave1.size(); ++i) {
        if (wave1[i].x >= wave2.front().x) {
            startIdx1 = i;
            break;
        }
    }

    // 寻找wave2中与wave1结束重叠的位置
    size_t endIdx2 = 0;
    for (size_t i = wave2.size() - 1; i > 0; --i) {
        if (wave2[i].x <= wave1.back().x) {
            endIdx2 = i;
            break;
        }
    }

    // 确保重叠区域有足够的点
    if (startIdx1 >= wave1.size() - 1 || endIdx2 <= 1) {
        return {wave1.size(), 0}; // 重叠区域太小
    }

    return {startIdx1, endIdx2};
}

// 波形平滑融合函数
std::vector<Point> smoothFusion(
    const std::vector<Point>& wave1,
    const std::vector<Point>& wave2,
    double offset) {

    // 1. 寻找重叠区域
    auto [startIdx1, endIdx2] = findOverlapRegion(wave1, wave2);

    // 如果没有重叠区域，直接拼接
    if (startIdx1 >= wave1.size() || endIdx2 == 0) {
        std::vector<Point> fusedWave = wave1;
        fusedWave.insert(fusedWave.end(), wave2.begin(), wave2.end());
        return fusedWave;
    }

    // 2. 创建融合区域
    std::vector<Point> fusedRegion;

    // 确定融合区域的起点和终点
    double startX = wave1[startIdx1].x;
    double endX = wave2[endIdx2].x;

    // 生成融合点（约每5单位一个点）
    size_t numPoints = static_cast<size_t>((endX - startX) / 5.0) + 1;
    for (size_t i = 0; i < numPoints; ++i) {
        double x = startX + i * 5.0;
        if (x > endX) break;

        // 在wave1中找到最近的2个点
        Point p1a, p1b;
        for (size_t j = startIdx1; j < wave1.size(); ++j) {
            if (wave1[j].x >= x) {
                p1b = wave1[j];
                p1a = (j > startIdx1) ? wave1[j - 1] : p1b;
                break;
            }
        }

        // 在wave2中找到最近的2个点
        Point p2a, p2b;
        for (size_t j = 0; j <= endIdx2; ++j) {
            if (wave2[j].x >= x) {
                p2b = wave2[j];
                p2a = (j > 0) ? wave2[j - 1] : p2b;
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
        double y = y1 * (1 - weight) + y2 * weight;

        fusedRegion.push_back({x, y});
    }

    // 3. 构建最终波形
    std::vector<Point> fusedWave;

    // wave1的前半部分（非重叠部分）
    for (size_t i = 0; i < startIdx1; ++i) {
        fusedWave.push_back(wave1[i]);
    }

    // 融合区域
    fusedWave.insert(fusedWave.end(), fusedRegion.begin(), fusedRegion.end());

    // wave2的后半部分（非重叠部分）
    for (size_t i = endIdx2 + 1; i < wave2.size(); ++i) {
        fusedWave.push_back(wave2[i]);
    }

    return fusedWave;
}

// 连接处平滑处理
void smoothConnection(std::vector<Point>& wave, size_t connectionPoint, double smoothingFactor = 0.5) {
    if (connectionPoint == 0 || connectionPoint >= wave.size() - 1) return;

    // 获取连接点周围的点
    Point& prev = wave[connectionPoint - 1];
    Point& current = wave[connectionPoint];
    Point& next = wave[connectionPoint + 1];

    // 计算平滑后的y值
    double smoothedY = (prev.y + current.y + next.y) / 3.0;

    // 应用平滑
    current.y = smoothedY * smoothingFactor + current.y * (1 - smoothingFactor);
}

// 处理波形：偏移、融合、拆分
std::pair<std::vector<Point>, std::vector<Point>> processWaveforms(
    const std::vector<Point>& wave1,
    const std::vector<Point>& wave2,
    double pbs, int m, int n) {

    // 1. 计算偏移量
    double offset = calculateOffset(pbs, m, n);

    // 2. 创建wave2的副本并应用偏移量
    std::vector<Point> wave2Adjusted = wave2;
    applyOffset(wave2Adjusted, offset);

    // 3. 平滑融合两个波形
    std::vector<Point> fusedWave = smoothFusion(wave1, wave2Adjusted, offset);

    // 4. 寻找连接点（融合区域的结束位置）
    size_t connectionPoint = 0;
    if (!wave1.empty() && !wave2Adjusted.empty()) {
        double connectionX = wave1.back().x;
        for (size_t i = 0; i < fusedWave.size(); ++i) {
            if (fusedWave[i].x >= connectionX) {
                connectionPoint = i;
                break;
            }
        }
    }

    // 5. 对连接处进行平滑处理
    if (connectionPoint > 0 && connectionPoint < fusedWave.size() - 1) {
        smoothConnection(fusedWave, connectionPoint);
    }

    // 6. 拆分波形 - 关键修改：分割点定为第一组的结束位置
    size_t splitPoint = 0;
    if (!wave1.empty()) {
        // 找到wave1结束的位置
        double endX = wave1.back().x;

        // 寻找融合波形中第一个x大于endX的点
        for (size_t i = 0; i < fusedWave.size(); ++i) {
            if (fusedWave[i].x > endX) {
                splitPoint = i;
                break;
            }
        }

        // 如果没有找到大于endX的点，说明整个波形都在wave1范围内
        if (splitPoint == 0) {
            splitPoint = fusedWave.size();
        }
    } else {
        splitPoint = 0; // 如果wave1为空，全部作为第二部分
    }

    // 7. 创建第一部分 - 包含所有在wave1结束位置之前的点
    std::vector<Point> firstPart(fusedWave.begin(), fusedWave.begin() + splitPoint);

    // 8. 创建第二部分并减去偏移量
    std::vector<Point> secondPart(fusedWave.begin() + splitPoint, fusedWave.end());
    applyOffset(secondPart, -offset);

    return {firstPart, secondPart};
}