//
// Created by 吴栋 on 2025/10/2.
//

#ifndef PARAMGENERATOR_WAVEPROCESSOR_H
#define PARAMGENERATOR_WAVEPROCESSOR_H
#include <utility>
#include <vector>

#include "../struct/Point.h"


std::pair<std::vector<Point>, std::vector<Point>> smoothFusion(
    const std::vector<Point>& wave1,
    const std::vector<Point>& wave2);


#endif //PARAMGENERATOR_WAVEPROCESSOR_H