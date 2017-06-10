//
//  main.cpp
//  Lab2
//
//  Created by Huijuan Zou on 10/29/16.
//  Copyright © 2016 Huijuan Zou. All rights reserved.
//

#include <fstream>
#include <iostream>
#include "ThreadPoolServer.hpp"

void showHelpInf(void);
void test();
static vector<string> output;
static bool verbose = false;
static bool runTest = false;

using namespace std;

int main(int argc, char * argv[]) {
    int option;
    bool hasOption = false;
    unsigned long numThreads = 0;
    streambuf* backup = cout.rdbuf();
    ofstream out("output.txt");
    cout.rdbuf(out.rdbuf());
    
    /*option of number of threads for commandline argument*/
    while ((option = getopt(argc, argv, "n:")) != -1) {
        if (!hasOption) {
            hasOption = true;
        }
        switch (option) {
            case 'n' : {
                numThreads = atoll(optarg);
                break;
            }
            default : {
                showHelpInf();
                break;
            }
        }
    }
    /*help information shown if ran without options*/
    if(!hasOption) {
        showHelpInf();
    }
    cout << "Number of threads: " << numThreads << endl;
    ThreadPoolServer server;
    server.run(htons(4060), (int) numThreads);
    cout.rdbuf(backup);
    out.close();
    return 0;
}

/**
 * This function is intended to show information to help users run the program.
 **/
void showHelpInf(void) {
    cout << "Please follow the instrution below. " << endl;
    cout << "./lab2 -n <N>" << endl;
}
