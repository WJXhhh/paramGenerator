//
// Created by Administrator on 2025/10/2.
//


#include <string>
#include "../json/json.hpp"

#ifndef PARAMGENERATOR_NOTE_NORMAL_H
#define PARAMGENERATOR_NOTE_NORMAL_H

using namespace std;
using namespace nlohmann;

struct note_normal {
    int posTick;
    string phonemes;
    int objID;
    int noteNum;
    //int phLock;
    int durTick;
    //string lyric;
    int velocity;
};

inline void from_json(const json& j, note_normal& c) {
    j.at("posTick").get_to(c.posTick);
    j.at("phonemes").get_to(c.phonemes);
    j.at("objID").get_to(c.objID);
    j.at("noteNum").get_to(c.noteNum);
    j.at("durTick").get_to(c.durTick);
    j.at("velocity").get_to(c.velocity);
}

inline void to_json(json& j, const note_normal& c) {
    j = json{
            {"posTick", c.posTick},
            {"phonemes", c.phonemes},
            {"objID", c.objID},
            {"noteNum", c.noteNum},
            {"durTick", c.durTick},
            {"velocity", c.velocity}
    };
}

#endif //PARAMGENERATOR_NOTE_NORMAL_H