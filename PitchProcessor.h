//
// Created by 吴栋 on 2025/10/2.
//

#ifndef PARAMGENERATOR_PITCHPROCESSOR_H
#define PARAMGENERATOR_PITCHPROCESSOR_H
#include <vector>

#include "Point.h"


std::pair<std::vector<Point>, std::vector<Point>> processWaveforms(
    const std::vector<Point>& wave1,
    const std::vector<Point>& wave2,
    double pbs, int m, int n);


#endif //PARAMGENERATOR_PITCHPROCESSOR_H