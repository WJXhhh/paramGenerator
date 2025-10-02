//
// Created by 吴栋 on 2025/10/2.
//

#include "vel_reasoner.h"
#include <vector>

#include "../struct/note_simple.h"

using namespace std;


vector<int> result(const vector<note_simple>& notes)
{
    bool hasPrev = false;
    bool hasNext = false;
    note_simple prev_note;
    note_simple next_note;
    for (int i = 0; i < notes.size(); i++) {
        note_simple note = notes[i];
        int dtl = 100000;
        int dtp = 0;
        if (i>0) {
            hasPrev = true;
            prev_note = notes[i-1];
        }
        if (i<notes.size()-1) {
            hasNext = true;
            next_note = notes[i+1];
        }
        if (hasPrev) {
            dtl = note.start - (prev_note.start + prev_note.duration);
            dtp = note.pitch - prev_note.pitch;
        }
        int vel = 64;

    }
}