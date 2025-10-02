//
// Created by 吴栋 on 2025/10/2.
//

#ifndef PARAMGENERATOR_WAVEGEN_H
#define PARAMGENERATOR_WAVEGEN_H
#include <vector>

#include "../struct/Point.h"


std::vector<Point> generateMountainWave(
    double startX, double endX,       // 起点和终点X坐标
    double peakStartX, double peakEndX, // 山峰起始和结束位置
    double startHeight,               // 起点高度
    double peakHeightOffset,          // 山峰高度相对于起点的增量
    double endHeight,                 // 终点高度
    int waveType,                     // 波形类型 (0:平缓, 1:凹陷, 2:凸起)
    double intensity = 0.0,           // 凹陷/凸起强度
    double width = 0.0,               // 凹陷/凸起宽度
    double position = 0.5,            // 凹陷/凸起位置 (0-1)
    bool allowNegative = true,         // 是否允许负值
    bool smoothBaseCurve = true,       // 是否平滑基础曲线
    double noiseIntensity = 0.05       // 噪声强度系数
);


#endif //PARAMGENERATOR_WAVEGEN_H