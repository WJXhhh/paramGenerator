//
// Created by 吴栋 on 2025/10/2.
//

#include "WaveGen.h"
#include "../struct/Point.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <set>
#include "../cons.h"

// 一维柏林噪声生成器
class PerlinNoise {
private:
    std::vector<int> p;

    double fade(double t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    double lerp(double t, double a, double b) {
        return a + t * (b - a);
    }

    double grad(int hash, double x) {
        int h = hash & 15;
        double grad = 1.0 + (h & 7);
        return (h & 8) ? -grad : grad;
    }

public:
    PerlinNoise() {
        p = {
            151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
            8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
            35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
            134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
            55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,
            18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
            250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
            189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
            172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,
            228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,
            107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
            138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
        };
        p.insert(p.end(), p.begin(), p.end());
    }

    double noise(double x) {
        int X = (int)floor(x) & 255;
        x -= floor(x);
        double u = fade(x);
        int A = p[X];
        int B = p[X+1];
        return lerp(u, grad(A, x), grad(B, x-1));
    }
};

// 高斯平滑滤波器（保护关键点）
std::vector<Point> gaussianSmoothWithKeyPoints(
    const std::vector<Point>& points,
    const std::set<size_t>& protectedIndices,
    double sigma = 2.0,
    int kernelSize = 5) {

    if (points.empty()) return points;

    // 创建高斯核
    std::vector<double> kernel;
    double sum = 0.0;

    int halfSize = kernelSize / 2;
    for (int i = -halfSize; i <= halfSize; ++i) {
        double value = std::exp(-(i * i) / (2 * sigma * sigma));
        kernel.push_back(value);
        sum += value;
    }

    // 归一化核
    for (double& value : kernel) {
        value /= sum;
    }

    // 应用高斯平滑（跳过保护点）
    std::vector<Point> smoothedPoints = points;

    for (size_t i = 0; i < points.size(); ++i) {
        // 如果是保护点，跳过平滑
        if (protectedIndices.find(i) != protectedIndices.end()) {
            continue;
        }

        double smoothedY = 0.0;
        int count = 0;

        for (int j = -halfSize; j <= halfSize; ++j) {
            int index = static_cast<int>(i) + j;
            if (index >= 0 && index < static_cast<int>(points.size())) {
                // 如果邻居是保护点，使用原始值
                if (protectedIndices.find(index) != protectedIndices.end()) {
                    smoothedY += points[index].y * kernel[j + halfSize];
                } else {
                    smoothedY += smoothedPoints[index].y * kernel[j + halfSize];
                }
                count++;
            }
        }

        // 边界处理
        if (count < kernelSize) {
            smoothedY *= static_cast<double>(kernelSize) / count;
        }

        smoothedPoints[i].y = smoothedY;
    }

    return smoothedPoints;
}

// 波形生成函数（保护关键点）
std::vector<Point> generateMountainWave(
    double startX, double endX,
    double peakStartX, double peakEndX,
    double startHeight,
    double peakHeightOffset,
    double endHeight,
    int waveType,
    double intensity,
    double width,
    double position,
    bool allowNegative,
    bool smoothBaseCurve,
    double noiseIntensity) {

    std::vector<Point> points;
    PerlinNoise pn;

    // 计算山峰绝对高度
    double peakHeight = startHeight + peakHeightOffset;

    // 噪声强度基于山峰高度
    double noiseStrength = std::abs(peakHeight) * noiseIntensity;

    // 确保参数有效性
    width = std::max(0.1, width);
    position = std::clamp(position, 0.0, 1.0);

    // 计算中心位置
    double centerX = peakStartX + (peakEndX - peakStartX) * position;

    // 生成点 (约每5单位一个点)
    for (double x = startX; x <= endX; x += 5.0) {
        double y = 0.0;

        // 上升段
        if (x < peakStartX) {
            double t = (x - startX) / (peakStartX - startX);
            y = startHeight + (peakHeight - startHeight) * t;
        }
        // 下降段
        else if (x > peakEndX) {
            double t = (x - peakEndX) / (endX - peakEndX);
            y = peakHeight + (endHeight - peakHeight) * t;
        }
        // 山峰段
        else {
            // 基础高度
            y = peakHeight;

            // 凹陷波形
            if (waveType == 1) {
                double distance = std::abs(x - centerX);
                if (distance < width/2) {
                    double t = distance / (width/2);
                    double depth = intensity * (1 - t*t);
                    y -= depth;
                }
            }
            // 凸起波形
            else if (waveType == 2) {
                double distance = std::abs(x - centerX);
                if (distance < width/2) {
                    double t = distance / (width/2);
                    double bulge = intensity * (1 - t*t);
                    y += bulge;
                }
            }
        }

        points.push_back({x, y});
    }

    // 识别关键点（起始点、终点、最高点）
    std::set<size_t> protectedIndices;

    // 起始点
    protectedIndices.insert(0);

    // 终点
    protectedIndices.insert(points.size() - 1);

    // 最高点（山峰中心点）
    size_t peakIndex = 0;
    double minDistance = std::numeric_limits<double>::max();

    for (size_t i = 0; i < points.size(); ++i) {
        double distance = std::abs(points[i].x - centerX);
        if (distance < minDistance) {
            minDistance = distance;
            peakIndex = i;
        }
    }
    protectedIndices.insert(peakIndex);

    // 平滑基础曲线（保护关键点）
    if (smoothBaseCurve) {
        points = gaussianSmoothWithKeyPoints(points, protectedIndices);
    }

    // 添加柏林噪声（不影响关键点）
    const double noiseScale = 0.01*drama;
    for (size_t i = 0; i < points.size(); ++i) {
        // 如果是关键点，不添加噪声
        if (protectedIndices.find(i) != protectedIndices.end()) {
            continue;
        }

        double noise = pn.noise(points[i].x * noiseScale) * noiseStrength;
        points[i].y += noise;

        // 如果不允许负值，确保y不小于0
        if (!allowNegative) {
            points[i].y = std::max(0.0, points[i].y);
        }
    }

    return points;
}