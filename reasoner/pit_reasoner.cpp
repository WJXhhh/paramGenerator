//
// Created by Administrator on 2025/10/3.
//

#include "pit_reasoner.h"

#include "../struct/note_simple.h"
#include "../cons.h"

#include <iostream>
#include <vector>
#include "../math/soft_diff.h"

#include "../generator/WaveGen.h"
#include "../processor/PitchProcessor.h"
#include "../struct/Point.h"

using namespace std;

vector<pair<int,double>> resultPIT (const vector<note_simple> &notes) {
    vector<vector<Point>> allv;


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

        vector<Point> ttv;

        double sig = 8192/pbs;



        if (dtl>=6||!hasPrev) {
            vector<Point> wv = generateMountainWave(note.start,note.start+note.duration,
                note.start+(note.duration*0),note.start+(note.duration*0.8),sig*(-2),1.5*sig,-1.5*sig,2,0.7*sig,0.6*note.duration,0.6,true,true);
            ttv = wv;

        }else if (hasPrev&&dtl<=1&&dtp>=1&&(prev_note.duration/tempo)>3) {
            vector<Point> wv = generateMountainWave(note.start,note.start+note.duration,
                note.start+(note.duration*0.2),note.start+(note.duration*0.7),sig*(-1.3),1.5*sig,-0.3*sig,2,0.4*sig,0.3*note.duration,0.5,true,true);
            ttv = wv;
        }
        else if (dtl<=0&&dtp>=1&&note.duration/tempo<=5) {
            vector<Point> wv = generateMountainWave(max(note.start-60,headPos),note.start+note.duration,
                max((note.start-60)+(note.duration*0.15),headPos),note.start+(note.duration*0.85),sig*(-1.7),1.3*sig,-2*sig,2,0.5*sig,0.7*note.duration,0.15,true,true);
            ttv = wv;
        }
        else if (dtl<=0&&dtp<=2&&note.duration/tempo>=6) {
            vector<Point> wv = generateMountainWave(max(note.start,headPos),note.start+note.duration,
                max((note.start)+(note.duration*0.4),headPos),note.start+(note.duration*1),sig*(-2),2*sig,0,0,0,0,0,true,true);
            ttv = wv;
        }
        else if (hasPrev&&dtl<=1&&dtp>=1&&(prev_note.duration/tempo)<=3) {
            vector<Point> wv = generateMountainWave(max(note.start-60,headPos),note.start+note.duration,
                max((note.start-60)+(note.duration*0.1),headPos),note.start+(note.duration*0.7),sig*(-1.7),1.6*sig,-0.3*sig,2,0.5*sig,0.3*note.duration,0.2,true,true);
            ttv = wv;
        }
        else if (dtl<=0&&dtp<=-1&&note.duration/tempo>=3&&note.duration/tempo<=6) {

            vector<Point> wv = generateMountainWave(max(note.start-60,headPos),note.start+note.duration,
                max((note.start-60)+(note.duration*0.2),headPos),note.start+(note.duration*0.7),sig*(-1.7),1.4*sig,-0.3*sig,2,0.5*sig,0.4*note.duration,0.3,true,true);
            ttv = wv;
        }

        else {
            vector<Point> wv = generateMountainWave(max(note.start,headPos),note.start+note.duration,
                max((note.start)+(note.duration*0.2),headPos),note.start+(note.duration*0.8),sig*-0.5,0.5*sig,-0.5*sig,0,0,0,0,true,true);
            ttv = wv;
        }

        for (int i = 0; i < ttv.size(); i++) {
            ttv[i].y /= 1.5;
            ttv[i].y *= drama;
            ttv[i].y = clamp(ttv[i].y,-8192.0,8192.0);
        }

        if (allv.size()>0) {
            vector<Point> prwv = allv[allv.size()-1];

            pair<vector<Point>,vector<Point>> pp = processWaveforms(prwv,ttv);
            allv[allv.size()-1] = pp.first;
            ttv = pp.second;
        }
        allv.push_back(ttv);

    }
    vector<pair<int,double>> rs;
    for (auto &item:allv) {
        for (auto &item2:item) {
            rs.push_back(make_pair(item2.x,item2.y));
        }
    }

    return rs;
}