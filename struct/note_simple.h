//
// Created by 吴栋 on 2025/10/2.
//

#include <string>
#include "../json/json.hpp"

#ifndef PARAMGENERATOR_NOTE_SIMPLE_H
#define PARAMGENERATOR_NOTE_SIMPLE_H

using namespace std;
using namespace nlohmann;

struct note_simple {
    double start;
    int pitch;
    double duration;
    string phoneme;
};

inline void from_json(const json& j, note_simple& c) {
    j.at("posTick").get_to(c.start);
    j.at("phonemes").get_to(c.phoneme);

    j.at("noteNum").get_to(c.pitch);
    j.at("durTick").get_to(c.duration);

}

inline void to_json(json& j, const note_simple& c) {
    j = json{
                {"posTick", c.start},
                {"phonemes", c.phoneme},

                {"noteNum", c.pitch},
                {"durTick", c.duration},

    };
}


#endif //PARAMGENERATOR_NOTE_SIMPLE_H