//
// Created by Administrator on 2025/10/3.
//

#include "../struct/note_simple.h"
#include "../cons.h"

#include <iostream>
#include <vector>
#include "../math/soft_diff.h"

#include "../generator/WaveGen.h"
#include "../processor/WaveProcessor.h"
#include "../struct/Point.h"

using namespace std;

#ifndef PARAMGENERATOR_PIT_REASONER_H
#define PARAMGENERATOR_PIT_REASONER_H


vector<pair<int,double>> resultPIT (const vector<note_simple> &notes);

#endif //PARAMGENERATOR_PIT_REASONER_H