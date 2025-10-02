#include <fstream>
#include <iostream>
#include "./json/json.hpp"
#include "./struct/note_normal.h"
#include "struct/Point.h"
#include "generator/WaveGen.h"
#include "processor/WaveProcessor.h"
#include "struct/note_simple.h"
#include "reasoner/vel_reasoner.h"
#include "reasoner/bre_reasoner.h"
#include "cons.h"

using namespace std;
using namespace nlohmann;

int main() {

    ifstream file("./noteExp.txt");
    if (!file.is_open()) {
        cout << "Failed to open file." << endl;
        return 1;
    }

    ifstream file2("./tempoExp.txt");
    if (!file2.is_open()) {
        cout << "Failed to open file." << endl;
        return 1;
    }

    //double tempo;

    file2 >> tempo;

    cout<<tempo<<endl;

    json Mj;
    file >> Mj;

    vector<note_normal> allNotes;

    vector<note_simple> allSNotes;

    if (Mj.is_array()) {
        for (auto& item : Mj) {

            {
                note_normal nt = item.get<note_normal>();
                note_simple sn = note_simple{
                        (double)nt.posTick,
                        nt.noteNum,
                        (double)nt.durTick,
                        nt.phonemes
                };
                allNotes.push_back(nt);
                allSNotes.push_back(sn);

            }
        }

        vector<pair<int,double>> rs = resultBRE(allSNotes);

        for (auto &item:rs) {
            //cout<<item.first<<" , "<<item.second<<endl;
        }




    }else {
        cout<<"noarray"<<endl;
    }




}