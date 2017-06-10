//
//  main.cpp
//  lab1
//
//  Created by Huijuan Zou on 10/7/16.
//  Copyright © 2016 Huijuan Zou. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include <random>
#include <fstream>
#include <thread>
#include "ThreadSafeKVStore.hpp"

/*arguments to be passed to the function that threads intended to run*/
struct arg_struct {
    ThreadSafeKVStore arg1;
    unsigned long arg2;
};

void showHelpInf(void);
void threadGenerator(const unsigned long&);
string valueGenerator(default_random_engine&);
void* storeManager(void* );
void test();
static vector<string> output;
static pthread_mutex_t printLock = PTHREAD_MUTEX_INITIALIZER;
static bool verbose = false;
static bool runTest = false;
using namespace std;

int main(int argc,  char * argv[]) {
    int option;
    bool hasOption = false;
    unsigned long numThreads = 0;
    streambuf* backup = cout.rdbuf();
    ofstream out("output.txt");
    cout.rdbuf(out.rdbuf());
    
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
    threadGenerator(numThreads);
    if (runTest) {
        test();
    }
    cout.rdbuf(backup);
    out.close();
    return 0;
}

/**
 * This function is intented to generate threads. Threads start to work from here.
 * @param numThreads total number of threads.
 **/
void threadGenerator(const unsigned long& numThreads) {
    ThreadSafeKVStore store;
    pthread_t thread[numThreads];
    /*running time for each thread*/
    double timeElapsed[numThreads];
    /*start time for each thread*/
    clock_t begin[numThreads];
    vector<arg_struct> argsVec;
    for (unsigned long i = 0; i < numThreads; i++) {
        struct arg_struct args;
        args.arg1 = store;
        args.arg2 = i;
        argsVec.push_back(args);
    }
    for (unsigned long i = 0; i < numThreads; i++) {
        begin[i] = clock();
        if (pthread_create(& thread[i], NULL, storeManager,(void*) & argsVec[i]) != 0){
            cout << "Oh-no, error in creating thread " << i << endl;
        }
    }
    for (unsigned long i = 0; i < numThreads; i++) {
        pthread_join(thread[i], NULL);
        timeElapsed[i] = double(clock() - begin[i]) / CLOCKS_PER_SEC;
    }
    cout << "Total " << "completion time "
        + to_string(((double)clock() - begin[0])/ CLOCKS_PER_SEC) << endl;
    for (unsigned long i = 0; i < numThreads; i++) {
        cout << "Thread " << i << "completion time " + to_string(timeElapsed[i]) << endl;
    }
}

/**
 * This function is intended for multiple threads to do operations on ThreadSafeKVStore.
 * Local variables are thread safe.
 * Standard output is protected by a mutex lock.
 * @ param arguments arguments include shared store information and thread id.
 **/
void* storeManager(void* arguments) {
    struct arg_struct args = *(struct arg_struct*) arguments;
    ThreadSafeKVStore store = args.arg1;
    unsigned long threadID = args.arg2;
    /*Thread id is used as random generator's seed to generate different 
        generators for different threads*/
    default_random_engine randomGenerator((int) threadID + 100);
    vector< pair<string,string> > pairs;
    uniform_int_distribution<int> keyDist(0 , 10000000);
    uniform_int_distribution<int> userDist(0 , 9);
    for (int j = 0; j < 10000; j++) {
        for (int k = 0; k < 10; k++) {
            /*r is used to generate different probabilities for different operations.
              Here if r == 0, we insert, if r == 1, we remove. we lookup for r == 2 ~ 9*/
            int  r = userDist(randomGenerator);
            string message = "";
            if (r < 1) {
                int userNum = keyDist(randomGenerator);
                string key = "user" + to_string(userNum);
                string value = valueGenerator(randomGenerator);
                int isInserted = store.insert(key, value);
                pairs.push_back(make_pair(key , value));
                /*if verbose is true, then all the messages will be printed*/
                if (isInserted == 0) {
                    message = "Insert: Thread " + to_string(threadID) + " " + key
                    + "\nValue: " + value +" succeed.";
                } else {
                    message = "Insert: Thread " + to_string(threadID) + " " + key
                    + "\nValue: " + value +" failed.";
                }
            } else if (r < 2) {
                if (pairs.size() >= 1) {
                    string curKey = pairs[rand() % pairs.size()].first;
                    int isRemoved = store.remove(curKey);
                    if (isRemoved == 0) {
                        message = "Remove: Thread " + to_string(threadID)
                        + " " + curKey + " succeed.";
                    } else {
                        message = "Remove: Thread " + to_string(threadID)
                        + " " + curKey + " failed.";
                    }
                } else {
                    message = "Remove: Thread " + to_string(threadID)
                    + " Store is empty. Nothing to be removed";
                }
            } else {
                if (pairs.size() >= 1) {
                    int chosenIndex = rand() % pairs.size();
                    string curKey = pairs[chosenIndex].first;
                    string curValue = pairs[chosenIndex].second;
                    /*Store the old value associated with the key. It will be overwritten
                     by the new value associated with the key if key is found.*/
                    string oldValue = curValue;
                    int found = store.lookup(curKey, curValue);
                    /*modifying pairs is thread safe here because the variable is thread local*/
                    pairs[chosenIndex].second = curValue;
                    if (found == 0) {
                        message = "LookUp: Thread " + to_string(threadID) + " "
                        + curKey + " is found.\nOld Value is: " + oldValue
                        +  "\nCurrent Value is: " + curValue;
                    } else {
                        message = "LookUp: Thread " + to_string(threadID) + " "
                        + curKey + " not found. Key has been removed.";
                    }
                } else {
                    message = "LookUp: Thread " + to_string(threadID)
                    + " Store is empty. Nothing to look up.";
                }
            }
            if (verbose) {
                pthread_mutex_lock(& printLock);
                cout << message << endl;
                cout << endl;
                pthread_mutex_unlock(& printLock);
            }
        }
    }
    return NULL;
}

/**
 * @ param randomGenerator default random engine seeded with thread id. 
 *   Each thread has its unique random generator.
 * @ return a lower case string. The length of this string has uniform 
 *   distribution between 8 and 256.
 *   The characters in the string also have uniform distribution.
 **/
string valueGenerator(default_random_engine& randomGenerator) {
    string value = "";
    uniform_int_distribution<int> lenDist(8, 256);
    uniform_int_distribution<int> charDist(97, 122);
    int len = lenDist(randomGenerator);
    for (int i = 0; i < len; i++) {
        value += (char) charDist(randomGenerator);
    }
    return value;
}

/**
 *  This function is for testing thread safety for insert/remove functions.
 **/
void* testStore(void* arguments) {
    struct arg_struct args = *(struct arg_struct*) arguments;
    ThreadSafeKVStore store = args.arg1;
    unsigned long threadID = args.arg2;
    int isInserted = store.insert("12345" , "khalndkff");
    int isInserted2 = store.insert("12345" , "ionknndd");
    int isRemoved = store.remove("12345");
    /*thread safe printing*/
    pthread_mutex_lock(& printLock);
    if (isInserted == 0) {
        cout << "Thread " << threadID << ": Test succeed. Key is inserted." << endl;
    } else {
        cout << "Thread " << threadID << ": Test failed. Key is not inserted." << endl;
    }
    if (isInserted2 == 0) {
        cout << "Thread " << threadID << ": Test succeed. New value is updated." << endl;
    } else {
        cout << "Thread " << threadID <<  ": Test failed. New value is not updated." << endl;
    }
    if (isRemoved == 0) {
        cout << "Thread " << threadID << ": Test succeed. Key is deleted." << endl;
    } else {
        cout << "Thread " << threadID << ": Test failed. Key is not deleted." << endl;
    }
    pthread_mutex_unlock(& printLock);
    return NULL;
}

/**
 * Test function for testing thread safety.
 **/
void test() {
    int numThreads = 10;
    ThreadSafeKVStore store;
    vector<arg_struct> argsVec;
    for (unsigned long i = 0; i < numThreads; i++) {
        struct arg_struct args;
        args.arg1 = store;
        args.arg2 = i;
        argsVec.push_back(args);
    }
    pthread_t thread[numThreads];
    for (unsigned long i = 0; i < numThreads; i++) {
        if (pthread_create(& thread[i], NULL, & testStore, (void*) & argsVec[i]) != 0){
            cout << "Oh-no, error in creating thread " << i << endl;
        }
    }
    for (unsigned long i = 0; i < numThreads; i++) {
        pthread_join(thread[i], NULL);
    }
}

/**
 * This function is intended to show information to help users run the program.
 **/
void showHelpInf(void) {
    cout << "Please follow the instrution below. " << endl;
    cout << "./lab1 -n <N>" << endl;
    cout << "Or to get verbose version: " << endl;
    cout << "./lab1 -n <N> -v" << endl;
}
