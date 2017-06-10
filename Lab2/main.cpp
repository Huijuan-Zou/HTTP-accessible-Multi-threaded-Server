//
//  main.cpp
//  Lab2
//
//  Created by Huijuan Zou on 10/29/16.
//  Copyright © 2016 Huijuan Zou. All rights reserved.
//

#include <fstream>
#include "ThreadPoolServer.hpp"

void showHelpInf(void);
void test();
static vector<string> output;
static bool verbose = false;
static bool runTest = false;

int main(int argc, char * argv[]) {
    int option;
    bool hasOption = false;
    unsigned long numThreads = 0;
    
    /*option of number of threads for commandline argument*/
    while ((option = getopt(argc, argv, "vtn:")) != -1) {
        if (!hasOption) {
            hasOption = true;
        }
        switch (option) {
            case 'n' : {
                numThreads = atoll(optarg);
                break;
            }
            case 'v' : {
                /*verbose is used to print extra run time messages*/
                verbose = true;
                break;
            }
            case 't' : {
                /*test is used to run extra test function */
                runTest = true;
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
    ThreadPoolServer server;
    server.run(htons(4050), (int) numThreads);
    return 0;
}

/**
 * This function is intended to show information to help users run the program.
 **/
void showHelpInf(void) {
    cout << "Please follow the instrution below. " << endl;
    cout << "./lab2 -n <N>" << endl;
}
