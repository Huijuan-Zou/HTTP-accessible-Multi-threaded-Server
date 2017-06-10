//
//  OnDiskStore.cpp
//  
//
//  Created by Huijuan Zou on 12/6/16.
//
//

#include "OnDiskStore.hpp"

static ThreadSafeKVStore cache;

struct Triplet {    
    int numInsert;
    int numDelete;
    int numLookUp;
};

static Triplet statistics;

OnDiskStore :: OnDiskStore() {
    statistics.numInsert = 0;
    statistics.numDelete = 0;
    statistics.numLookUp = 0;
}

OnDiskStore :: ~OnDiskStore() {
}

/**
 * This insert function is thread safe. A write lock is used
 * to protect the insert operation.
 * @ param key string to be inserted to the disk/cache as key.
 * @ param value string to be inserted to the cacheas value.
 *   If key already exists, then change the value associated
 *   with that key to passed in value.
 * @ param dir directory to on disk storage.
 * @ return an int value. If 0, the insert operation is successful.
 *   if -1, error occured during the insert operation.
 **/
int OnDiskStore :: insert(const string key, const string value, string dir) {
    try {
        string fileName = dir + key + ".txt";
        ofstream myfile;
        myfile.open(fileName.c_str());
        myfile << value;
        myfile.close();
        statistics.numInsert++;
    } catch (const runtime_error& error) {
        return -1;
    }
    return cache.insert(key, value);
}

/**
 * This lookup function is thread safe. A read lock is used to
 * protect the look up operation. Multiple read operations can
 * happen at the same time.
 * @ param key string to be looked up from the disk storage/cache.                     
 * @ param value string to be overwritten if key is found
 *   and current value associated with the key is different.
 * @ param dir directory to on disk storage.
 * @ return an int value. If 0, key is found.
 *   if -1, key is not found.
 **/
int OnDiskStore :: lookup(const string key, string& value,  string dir){
    string fileName = dir + key + ".txt";
    bool exist = true;
    string value1 = "b";
    string value2 = "";
    if (cache.lookup(key, value1) < 0) {
        ifstream file(fileName.c_str());
        if (file.is_open()){
            string str = "";
            string line;
            while (getline (file,line)){
                str += line;
            }
            file.close();
            value = str;
            return 0;
        } else { 
            exist = false;
        }
        if (key != "STA") {
            statistics.numLookUp++;
        }
    } 
    if (key == "STA") {
        value2 = "Number of insert operations on disk: " + to_string(statistics.numInsert) + "\nNumber of delete operations on disk: "
            + to_string(statistics.numDelete) + "\nNumber of lookup operations on disk: " + to_string(statistics.numLookUp);
        value = value1 + "\n" + value2;
    } 
    if (!exist) {
        return -1;
    }
    return 0;
}

/**
 * This remove function is thread safe. A write lock is used to
 * protect the remove operation.
 * @ param key string to be removed from the disk storage/cache.
 * @ param dir directory to on disk storage.
 * @ return an int value. If 0, the remove operation is successful.
 *   if -1, error occured during the remove operation.
 **/
int OnDiskStore :: remove(const string key,  string dir){
    string fileName = dir + key + ".txt";
     try {
         if (std :: remove(fileName.c_str()) != 0) {
             return -1;
         } 
         statistics.numDelete++;
     } catch (const runtime_error& error) {
         return -1;
     }
    return cache.remove(key);
}