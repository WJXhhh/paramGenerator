//
// Created by 吴栋 on 2025/10/2.
//

#include "PitchProcessor.h"

#include <vector>
#include <cmath>
#include <map>

#include "../struct/Point.h"

double lerpp(double a, double b, double t) {
    return a + t * (b - a);
}

// 计算偏移量（带偏移版本）
double calculateOffsetWithOffset(double pbs, int m, int n) {
    if (std::abs(pbs) < 1e-10) return 0.0; // 防止除零
    return (8192.0 / pbs) * (n - m);
}

// 应用偏移量到波形（带偏移版本）
void applyOffsetWithOffset(std::vector<Point>& wave, double offset) {
    for (auto& point : wave) {
        point.y += offset;
    }
}

// 寻找交叉区域（带偏移版本）
std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t>> findOverlapRegionWithOffset(
    const std::vector<Point>& wave1,
    const std::vector<Point>& wave2,
    double minSlopeThreshold = 0.01,
    int windowSize = 5) {

    // 1. 寻找wave1下降段的起始点（使用滑动窗口平均）
    size_t fallStart1 = wave1.size() - 1;
    if (wave1.size() > 2 * windowSize) {
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
            double dx = wave1[i + windowSize].x - wave1[i - windowSize].x;
            if (std::abs(dx) < 1e-10) continue;

            double slope = (rightAvg - leftAvg) / dx;

            // 检测下降趋势
            if (slope < -minSlopeThreshold) {
                fallStart1 = i;
                break;
            }
        }
    }

    // 2. 寻找wave2上升段的结束点（使用滑动窗口平均）
    size_t riseEnd2 = 0;
    if (wave2.size() > 2 * windowSize) {
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
            double dx = wave2[i + windowSize].x - wave2[i - windowSize].x;
            if (std::abs(dx) < 1e-10) continue;

            double slope = (rightAvg - leftAvg) / dx;

            // 检测上升趋势结束（开始下降）
            if (slope < minSlopeThreshold) {
                riseEnd2 = i;
                break;
            }
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

// 去除重复点（带偏移版本）
std::vector<Point> removeDuplicatePointsWithOffset(const std::vector<Point>& points) {
    if (points.empty()) return points;

    std::vector<Point> uniquePoints;
    std::map<double, Point> pointMap; // 使用map确保x坐标唯一

    for (const auto& p : points) {
        // 如果x坐标不存在，或者y坐标更接近原始值，则更新
        if (pointMap.find(p.x) == pointMap.end() ||
            std::abs(p.y - pointMap[p.x].y) < 1e-5) {
            pointMap[p.x] = p;
        }
    }

    // 提取唯一点（按x坐标排序）
    for (const auto& kv : pointMap) {
        uniquePoints.push_back(kv.second);
    }

    return uniquePoints;
}

// 波形平滑融合函数（带偏移版本）
std::vector<Point> smoothFusionWithOffset(
    const std::vector<Point>& wave1,
    const std::vector<Point>& wave2) {

    // 1. 寻找重叠区域
    auto [overlap1, overlap2] = findOverlapRegionWithOffset(wave1, wave2);
    auto [startIdx1, endIdx1] = overlap1;
    auto [startIdx2, endIdx2] = overlap2;

    // 如果没有重叠区域，直接拼接
    if (startIdx1 >= wave1.size() || endIdx2 == 0 ||
        startIdx1 >= endIdx1 || startIdx2 >= endIdx2) {
        std::vector<Point> fusedWave = wave1;
        fusedWave.insert(fusedWave.end(), wave2.begin(), wave2.end());
        return removeDuplicatePointsWithOffset(fusedWave);
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
        bool found1 = false;
        for (size_t j = startIdx1; j < wave1.size(); ++j) {
            if (wave1[j].x >= x) {
                p1b = wave1[j];
                p1a = (j > startIdx1) ? wave1[j - 1] : p1b;
                found1 = true;
                break;
            }
        }

        // 在wave2中找到最近的2个点
        Point p2a, p2b;
        bool found2 = false;
        for (size_t j = 0; j <= endIdx2; ++j) {
            if (wave2[j].x >= x) {
                p2b = wave2[j];
                p2a = (j > 0) ? wave2[j - 1] : p2b;
                found2 = true;
                break;
            }
        }

        double y1 = 0.0;
        double y2 = 0.0;

        // 插值得到y值（带除零保护）
        if (found1) {
            if (std::abs(p1a.x - p1b.x) < 1e-10) {
                y1 = p1a.y; // 使用最近点
            } else {
                double t1 = (x - p1a.x) / (p1b.x - p1a.x);
                y1 = p1a.y + t1 * (p1b.y - p1a.y);
            }
        }

        if (found2) {
            if (std::abs(p2a.x - p2b.x) < 1e-10) {
                y2 = p2a.y; // 使用最近点
            } else {
                double t2 = (x - p2a.x) / (p2b.x - p2a.x);
                y2 = p2a.y + t2 * (p2b.y - p2a.y);
            }
        }

        // 计算权重 - 基于在融合区域中的位置
        double weight = static_cast<double>(i) / (numPoints - 1);
        weight = 0.5 - 0.5 * std::cos(weight * M_PI); // 平滑的权重过渡

        // 融合y值（带边界检查）
        double y = 0.0;
        if (found1 && found2) {
            y = y1 * (1 - weight) + y2 * weight;
        } else if (found1) {
            y = y1;
        } else if (found2) {
            y = y2;
        } else {
            // 如果两个波形都没找到点，使用线性插值
            y = wave1[startIdx1].y + (x - startX) / (endX - startX) *
                (wave2[endIdx2].y - wave1[startIdx1].y);
        }

        fusedRegion.push_back({x, y});
    }

    // 3. 构建最终波形（避免重复点）
    std::vector<Point> fusedWave;

    // wave1的前半部分（非重叠部分）
    for (size_t i = 0; i < startIdx1; ++i) {
        fusedWave.push_back(wave1[i]);
    }

    // 融合区域（去除可能的重复点）
    if (!fusedWave.empty() && !fusedRegion.empty() &&
        std::abs(fusedWave.back().x - fusedRegion.front().x) < 1e-5) {
        // 如果最后一个点与融合区域第一个点重复，跳过融合区域第一个点
        fusedWave.insert(fusedWave.end(), fusedRegion.begin() + 1, fusedRegion.end());
    } else {
        fusedWave.insert(fusedWave.end(), fusedRegion.begin(), fusedRegion.end());
    }

    // wave2的后半部分（非重叠部分）
    if (!fusedWave.empty() && !wave2.empty() && endIdx2 + 1 < wave2.size() &&
        std::abs(fusedWave.back().x - wave2[endIdx2 + 1].x) < 1e-5) {
        // 如果最后一个点与wave2第一个点重复，跳过wave2第一个点
        for (size_t i = endIdx2 + 2; i < wave2.size(); ++i) {
            fusedWave.push_back(wave2[i]);
        }
    } else {
        for (size_t i = endIdx2 + 1; i < wave2.size(); ++i) {
            fusedWave.push_back(wave2[i]);
        }
    }

    // 最终去重
    return removeDuplicatePointsWithOffset(fusedWave);
}

// 连接处平滑处理（带偏移版本）
void smoothConnectionWithOffset(std::vector<Point>& wave, size_t connectionPoint, double smoothingFactor = 0.5) {
    if (connectionPoint == 0 || connectionPoint >= wave.size() - 1) return;

    // 获取连接点周围的点
    Point& prev = wave[connectionPoint - 1];
    Point& current = wave[connectionPoint];
    Point& next = wave[connectionPoint + 1];

    // 计算平滑后的y值（带除零保护）
    double smoothedY = current.y;

    // 检查点是否有效
    if (std::isfinite(prev.y) && std::isfinite(current.y) && std::isfinite(next.y)) {
        smoothedY = (prev.y + current.y + next.y) / 3.0;
    } else if (std::isfinite(prev.y) && std::isfinite(current.y)) {
        smoothedY = (prev.y + current.y) / 2.0;
    } else if (std::isfinite(current.y) && std::isfinite(next.y)) {
        smoothedY = (current.y + next.y) / 2.0;
    }

    // 应用平滑
    current.y = smoothedY * smoothingFactor + current.y * (1 - smoothingFactor);
}

// 处理波形：偏移、融合、拆分（带偏移版本）
std::pair<std::vector<Point>, std::vector<Point>> processWaveformsWithOffset(
    const std::vector<Point>& wave1,
    const std::vector<Point>& wave2,
    double pbs, int m, int n) {

    // 1. 计算偏移量
    double offset = calculateOffsetWithOffset(pbs, m, n);

    // 2. 创建wave2的副本并应用偏移量
    std::vector<Point> wave2Adjusted = wave2;
    applyOffsetWithOffset(wave2Adjusted, offset);

    // 3. 平滑融合两个波形
    std::vector<Point> fusedWave = smoothFusionWithOffset(wave1, wave2Adjusted);

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
        smoothConnectionWithOffset(fusedWave, connectionPoint);
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

    // 7. 创建第一部分
    std::vector<Point> firstPart(fusedWave.begin(), fusedWave.begin() + splitPoint);

    // 8. 创建第二部分并减去偏移量
    std::vector<Point> secondPart(fusedWave.begin() + splitPoint, fusedWave.end());
    applyOffsetWithOffset(secondPart, -offset);

    // 最终去重
    return {
        removeDuplicatePointsWithOffset(firstPart),
        removeDuplicatePointsWithOffset(secondPart)
    };
}