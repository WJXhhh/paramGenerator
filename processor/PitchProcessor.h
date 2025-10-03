//
// Created by 吴栋 on 2025/10/2.
//

#ifndef PARAMGENERATOR_PITCHPROCESSOR_H
#define PARAMGENERATOR_PITCHPROCESSOR_H
#include <vector>

#include "../struct/Point.h"


std::pair<std::vector<Point>, std::vector<Point>> processWaveformsWithOffset(
    const std::vector<Point>& wave1,
    const std::vector<Point>& wave2,
    double pbs, int m, int n);

double lerpw(double a, double b, double t);


#endif //PARAMGENERATOR_PITCHPROCESSOR_H