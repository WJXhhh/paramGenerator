//
// Created by 吴栋 on 2025/10/3.
//
#include <iostream>
#include <vector>
#include "../math/soft_diff.h"

#include "../generator/WaveGen.h"
#include "../processor/WaveProcessor.h"
#include "../struct/Point.h"
#include "../struct/note_simple.h"

using namespace std;
#ifndef PARAMGENERATOR_BRI_REASONER_H
#define PARAMGENERATOR_BRI_REASONER_H


vector<pair<int,double>> resultBRI (const vector<note_simple> &notes);


#endif //PARAMGENERATOR_BRI_REASONER_H