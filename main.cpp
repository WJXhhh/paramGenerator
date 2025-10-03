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
#include "reasoner/bri_reasoner.h"
#include "reasoner/pit_reasoner.h"

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

        vector<pair<int,double>> rsbri = resultBRI(allSNotes);
        vector<pair<int,double>> rsbre = resultBRE(allSNotes);
        vector<double> rsvel = resultVEL(allSNotes);


        // 输出 rsbri 到 rsbri.txt
        ofstream briFile("rsbri.txt");
        if (briFile.is_open()) {
            for (const auto& item : rsbri) {
                briFile << item.first << "," << round(item.second) << endl;
            }
            briFile.close();
        } else {
            cout << "Failed to create rsbri.txt" << endl;
        }

        // 输出 rsbre 到 rsbre.txt
        ofstream breFile("rsbre.txt");
        if (breFile.is_open()) {
            for (const auto& item : rsbre) {
                breFile << item.first << "," << round(item.second) << endl;
            }
            breFile.close();
        } else {
            cout << "Failed to create rsbre.txt" << endl;
        }

        // 输出 rsvel 到 rsvel.txt
        ofstream velFile("rsvel.txt");
        if (velFile.is_open()) {
            for (const auto& item : rsvel) {
                velFile << round(item) << endl;
            }
            velFile.close();
        } else {
            cout << "Failed to create rsvel.txt" << endl;
        }

        //输出 rspit 到 rspit.txt
        vector<pair<int,double>> rspit = resultPIT(allSNotes);
        ofstream pitFile("rspit.txt");
        if (pitFile.is_open()) {
            for (const auto& item : rspit) {
                pitFile << item.first << "," << round(item.second) << endl;
            }
            pitFile.close();
        } else {
            cout << "Failed to create rspit.txt" << endl;
        }


        cout<<"Completed!"<<endl;



    }else {
        cout<<"noarray"<<endl;
    }




}