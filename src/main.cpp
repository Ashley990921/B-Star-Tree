#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <list>
#include <random>
#include "bstar.h"
using namespace std;

int main(int argc, char** argv){
    fstream input_blk, input_net, output;
    double alpha;
    if (argc == 5) {
        alpha = stod(argv[1]);
        input_blk.open(argv[2], ios::in);
        input_net.open(argv[3], ios::in);
        output.open(argv[4], ios::out);
        if (!input_blk) {
            cerr << "Cannot open the input file \"" << argv[2]
                 << "\". The program will be terminated..." << endl;
            exit(1);
        }
        if (!input_net) {
            cerr << "Cannot open the input file \"" << argv[3]
                 << "\". The program will be terminated..." << endl;
            exit(1);
        }
        if (!output) {
            cerr << "Cannot open the output file \"" << argv[4]
                 << "\". The program will be terminated..." << endl;
            exit(1);
        }
    }
    else {
        cerr << "Usage: ./Floorplanner <alpha> <input block file> " <<
                "<input net file> <output file>" << endl;
        exit(1);
    }

    clock_t start, end;
    start=clock();

    Floorplanner* FP = new Floorplanner(alpha);
    FP->parse_blk(input_blk);
    FP->parse_net(input_net);
    FP->set_node();
    FP->SA();

    end=clock();
    double totaltime=double(end-start)/CLOCKS_PER_SEC;

    FP->writereport(totaltime, output);

    return 0;
}
