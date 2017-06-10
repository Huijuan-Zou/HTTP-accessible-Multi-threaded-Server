//
//  ThreadSafeKVStore.cpp
//  Lab2
//
//  Created by Huijuan Zou on 10/29/16.
//  Copyright © 2016 Huijuan Zou. All rights reserved.
//

#include "ThreadSafeKVStore.hpp"

struct Triplet {    
    int numInsert;
    int numDelete;
    int numLookUp;
};

static Triplet statistics;
unordered_map<string, string> myStore;

ThreadSafeKVStore :: ThreadSafeKVStore() {
    statistics.numInsert = 0;
    statistics.numDelete = 0;
    statistics.numLookUp = 0;
}

ThreadSafeKVStore :: ~ThreadSafeKVStore() {
}

/**
 * This insert function is thread safe. A write lock is used
 * to protect the insert operation.
 * @ param key string to be inserted to the map as key.
 * @ param value string to be inserted to the map as value.
 *   If key already exists, then change the value associated
 *   with that key to passed in value.
 * @ return an int value. If 0, the insert operation is successful.
 *   if -1, error occured during the insert operation.
 **/
int ThreadSafeKVStore :: insert(const string key, const string value) {
    try {
        pthread_rwlock_wrlock( & rwLock);
        bool isInserted = myStore.insert(make_pair(key, value)).second;
        statistics.numInsert++;
        myStore[key] = value;
        pthread_rwlock_unlock( & rwLock);
    } catch (const runtime_error& error) {
        return -1;
    }
    return 0; 
}

/**
 * This lookup function is thread safe. A read lock is used to
 * protect the look up operation. Multiple read operations can
 * happen at the same time.
 * @ param key string to be looked up from the map.                     
 * @ param value string to be overwritten if key is found
 *   and current value associated with the key is different.
 * @ return an int value. If 0, key is found.
 *   if -1, key is not found.
 **/
int ThreadSafeKVStore :: lookup(const string key, string& value){
    pthread_rwlock_rdlock( & rwLock);
    bool exist = true;
    unordered_map<string, string> :: const_iterator found = myStore.find(key);
    
    if (found == myStore.end()) {
        exist = false;
    } else {
        value = myStore[key];
    }
    if (key == "STA") {
        value = "Number of insert operations: " + to_string(statistics.numInsert) + "\nNumber of delete operations: "
             + to_string(statistics.numDelete) + "\nNumber of lookup operations: " + to_string(statistics.numLookUp);
    }  else {
        statistics.numLookUp++;
    }
    pthread_rwlock_unlock( & rwLock);
    if (!exist) {
        return -1;
    }
    return 0;
}

/**
 * This remove function is thread safe. A write lock is used to
 * protect the remove operation.
 * @ param key string to be removed from the map.
 * @ return an int value. If 0, the remove operation is successful.
 *   if -1, error occured during the remove operation.
 **/
int ThreadSafeKVStore :: remove(const string key){
    try {
        pthread_rwlock_wrlock( & rwLock);
        myStore.erase(key);
        statistics.numDelete++;
        pthread_rwlock_unlock( & rwLock);
    } catch (const runtime_error& error) {
        return -1;
    }
    return 0;
}
