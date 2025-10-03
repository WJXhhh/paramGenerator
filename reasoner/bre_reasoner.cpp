//
// Created by Administrator on 2025/10/2.
//

#include "../struct/note_simple.h"
#include "../cons.h"
#include "bre_reasoner.h"

#include <iostream>
#include <vector>
#include "../math/soft_diff.h"

#include "../generator/WaveGen.h"
#include "../processor/WaveProcessor.h"
#include "../struct/Point.h"

using namespace std;

vector<pair<int,double>> resultBRE (const vector<note_simple> &notes) {

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

        /*if (dtl >= 8 || (!hasPrev)) {
            double height = 15+((softDiffAtan<double>(note.pitch,60,80,1))*1);
            double ph = 10+((softDiffAtan<double>(note.pitch,60,50,1))*2);
            vector<Point> wv = generateMountainWave(
            note.start - 20, note.start + note.duration + 20,
            note.start+(0.2*note.duration),note.start+(0.8*note.duration),
            10,height,10,2,ph,0.4*note.duration,0.2,false,true

            );
            ttv = wv;
        }else if (dtp<=-3) {
            double height = 15+((softDiffAtan<double>(note.pitch,60,80,1))*1);
            double ph = 5+((softDiffAtan<double>(note.pitch,60,50,1))*2);
            vector<Point> wv = generateMountainWave(
            note.start - 20, note.start + note.duration + 20,
            note.start+(0.2*note.duration),note.start+(0.8*note.duration),
            10,height,10,1,ph,0.2*note.duration,0.2,false,true

            );
            ttv = wv;
        }
        else if (dtp>=3) {
            double height = 15+((softDiffAtan<double>(note.pitch,60,80,1))*1);
            double ph = 5+((softDiffAtan<double>(note.pitch,60,50,1))*2);
            vector<Point> wv = generateMountainWave(
            note.start - 20, note.start + note.duration + 20,
            note.start+(0.2*note.duration),note.start+(0.8*note.duration),
            10,height,10,2,ph,0.2*note.duration,0.2,false,true

            );
            ttv = wv;
        }
        else {
            double height = 12+((softDiffAtan<double>(note.pitch,60,80,1))*1);
            double ph = 5+((softDiffAtan<double>(note.pitch,60,50,1))*2);
            vector<Point> wv = generateMountainWave(
            note.start - 20, note.start + note.duration + 20,
            note.start+(0.2*note.duration),note.start+(0.8*note.duration),
            10,height,10,0,0,0,0,false,true

            );
            ttv = wv;
        }

        if (ttv.size()>0) {
            vector<Point> prwv = allv[allv.size()-1];
            pair<vector<Point>,vector<Point>> pp = smoothFusion(prwv,ttv);
            allv[allv.size()-1] = pp.first;
            ttv = pp.second;
        }*/
       double height = 12+((softDiffAtan<double>(note.pitch,60,80,1))*1);
       double ph = 5+((softDiffAtan<double>(note.pitch,60,50,1))*2);
       vector<Point> wv = generateMountainWave(
       note.start - 20, note.start + note.duration + 20,
       note.start+(0.2*note.duration),note.start+(0.8*note.duration),
       10,height,10,0,0,0,0,false,true

       );
       ttv = wv;

       if (allv.size()>0&&allv.size()<10) {
           vector<Point> prwv = allv[allv.size()-1];
           int bkk = ttv.size();
           pair<vector<Point>,vector<Point>> pp = processWaveforms(prwv,ttv);
           allv[allv.size()-1] = pp.first;
           ttv = pp.second;
           //cout<<bkk<<" "<<ttv.size()<<endl;
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

