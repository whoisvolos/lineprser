#include <iostream>
#include <string>
#include <fstream>
#include "LineParser.h"

using namespace std;

timespec sleep_intr { 0, 10000000 };

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Please set filename as 1st argument" << endl;
        return 1;
    }
    const char* fname = argv[1];
    cout << "Using file " << fname << endl;

    ifstream ifs;
    ifs.open(fname);
    if (!ifs.is_open()) {
        cerr << "Can not open file " << fname << endl;
        return 1;
    }

    string line;
    LineParser lp;
    bool running = true;
    streampos pos = 0l;

    while (running) {
        if (!getline(ifs, line) || ifs.eof()) {
            ifs.clear();
            ifs.seekg(pos);
            nanosleep(&sleep_intr, NULL);
            continue;
        }
        pos = ifs.tellg();
        running = !lp.onLine(line);
        if (!running && lp.isError()) {
            cerr << lp.getErrorStr() << endl;
        }
    }

    ifs.close();
    return 0;
}