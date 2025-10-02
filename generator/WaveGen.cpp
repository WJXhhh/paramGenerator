//
// Created by 吴栋 on 2025/10/2.
//

#include "../WaveGen.h"
#include "../struct/Point.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

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
        double grad = 1.0 + (h & 7); // 梯度值1-8
        return (h & 8) ? -grad : grad; // 随机方向
    }

public:
    PerlinNoise() {
        // 初始化排列表
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
        // 双倍排列表
        p.insert(p.end(), p.begin(), p.end());
    }

    double noise(double x) {
        // 网格单元
        int X = (int)floor(x) & 255;

        // 相对网格坐标
        x -= floor(x);

        // 计算加权参数
        double u = fade(x);

        // 哈希坐标
        int A = p[X];
        int B = p[X+1];

        // 插值
        return lerp(u, grad(A, x), grad(B, x-1));
    }
};

// 高斯平滑滤波器
std::vector<Point> gaussianSmooth(const std::vector<Point>& points, double sigma = 2.0, int kernelSize = 5) {
    if (points.empty()) return points;

    // 创建高斯核
    std::vector<double> kernel;
    double sum = 0.0;

    // 计算高斯核
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

    // 应用高斯平滑
    std::vector<Point> smoothedPoints = points;

    for (size_t i = 0; i < points.size(); ++i) {
        double smoothedY = 0.0;
        int count = 0;

        for (int j = -halfSize; j <= halfSize; ++j) {
            int index = static_cast<int>(i) + j;
            if (index >= 0 && index < static_cast<int>(points.size())) {
                smoothedY += points[index].y * kernel[j + halfSize];
                count++;
            }
        }

        // 如果边界点没有足够的邻居，调整权重
        if (count < kernelSize) {
            smoothedY *= static_cast<double>(kernelSize) / count;
        }

        smoothedPoints[i].y = smoothedY;
    }

    return smoothedPoints;
}

// 波形生成函数（山峰高度基于起始点）
std::vector<Point> generateMountainWave(
    double startX, double endX,       // 起点和终点X坐标
    double peakStartX, double peakEndX, // 山峰起始和结束位置
    double startHeight,               // 起点高度
    double peakHeightOffset,          // 山峰高度相对于起点的增量
    double endHeight,                 // 终点高度
    int waveType,                     // 波形类型 (0:平缓, 1:凹陷, 2:凸起)
    double intensity,           // 凹陷/凸起强度
    double width,               // 凹陷/凸起宽度
    double position,            // 凹陷/凸起位置 (0-1)
    bool allowNegative,         // 是否允许负值
    bool smoothBaseCurve,       // 是否平滑基础曲线
    double noiseIntensity       // 噪声强度系数
) {
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
                    double t = distance / (width/2); // 0-1
                    double depth = intensity * (1 - t*t); // 二次曲线凹陷
                    y -= depth;
                }
            }
            // 凸起波形
            else if (waveType == 2) {
                double distance = std::abs(x - centerX);
                if (distance < width/2) {
                    double t = distance / (width/2); // 0-1
                    double bulge = intensity * (1 - t*t); // 二次曲线凸起
                    y += bulge;
                }
            }
        }

        points.push_back({x, y});
    }

    // 平滑基础曲线
    if (smoothBaseCurve) {
        points = gaussianSmooth(points);
    }

    // 添加柏林噪声
    const double noiseScale = 0; // 噪声缩放系数
    for (auto& point : points) {
        double noise = pn.noise(point.x * noiseScale) * noiseStrength;
        point.y += noise;

        // 如果不允许负值，确保y不小于0
        if (!allowNegative) {
            point.y = std::max(0.0, point.y);
        }
    }

    return points;
}