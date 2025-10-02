//
// Created by 吴栋 on 2025/10/2.
//

#include "vel_reasoner.h"

#include <algorithm>
#include <map>
#include <cmath>
#include <iostream>
#include <vector>

#include "../struct/note_simple.h"

#include "../cons.h"


using namespace std;

map<string, double> pho_spe_down = {
    {"ts\\", 0.8},
    {"y", 1.2},

};

map<string, double> pho_spe_up = {
    {"ts\\", 0.8},
    {"y", 1.2},

};

vector<double> resultVEL(const vector<note_simple> &notes) {
    vector<double> rs;

    for (int i = 0; i < notes.size(); i++) {
        bool hasPrev = false;
        bool hasNext = false;
        note_simple prev_note;
        note_simple next_note;
        note_simple note = notes[i];
        int dtl = 100000;
        int dtp = 0;
        if (i > 0) {
            hasPrev = true;
            prev_note = notes[i - 1];
        }
        if (i < notes.size() - 1) {
            hasNext = true;
            next_note = notes[i + 1];
        }
        if (hasPrev) {
            dtl = (note.start - (prev_note.start + prev_note.duration))/tempo;
            dtp = note.pitch - prev_note.pitch;
        }


        double vel = 64;
        //cout<<dtl<<endl;
        if (dtl >= 3 || (!hasPrev)) {
            double offset = 1;
            for (auto &it: pho_spe_down) {
                string mapp = it.first;
                double spe = it.second;
                if (note.phoneme.starts_with(mapp)) {
                    offset = spe;
                    break;
                }
            }
            if (dtl <= 5) {
                vel = 64 - ((dtl - 2) * 9 * offset);
            } else {
                vel = 64 - (3 * 9 * offset);
            }
            vel = clamp(vel, 0.0, 127.0);
            rs.push_back(vel);
        } else if (hasPrev && dtp >= 4 && dtl <= 2) {
            double offset = 1;
            for (auto &it: pho_spe_down) {
                string mapp = it.first;
                double spe = it.second;
                if (note.phoneme.starts_with(mapp)) {
                    offset = spe;
                    break;
                }
            }
            vel *= 0.7 * offset;
            vel = clamp(vel, 0.0, 127.0);
            rs.push_back(vel);
        } else if (hasPrev && dtl <= 1) {
            if (prev_note.duration/tempo <= 2) {
                double offset = 1;
                for (auto &it: pho_spe_up) {
                    string mapp = it.first;
                    double spe = it.second;
                    if (note.phoneme.starts_with(mapp)) {
                        offset = spe;
                        break;
                    }
                }
                vel *= 1.2 * offset;
                vel = clamp(vel, 0.0, 127.0);
                rs.push_back(vel);
            }else {
                rs.push_back(vel);
            }
        } else {
            rs.push_back(vel);
        }
    }
    return rs;
}
