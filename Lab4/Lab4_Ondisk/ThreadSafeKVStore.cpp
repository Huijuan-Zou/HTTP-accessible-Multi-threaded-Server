//
//  ThreadSafeKVStore.cpp
//  Lab2
//
//  Created by Huijuan Zou on 10/29/16.
//  Copyright © 2016 Huijuan Zou. All rights reserved.
//  LRU cache is implementated in this thread safe store.
//

#include "ThreadSafeKVStore.hpp"

struct Triplet {    
    int numInsert;
    int numDelete;
    int numLookUp;
};

static Triplet statistics;

ThreadSafeKVStore :: ThreadSafeKVStore() {
    statistics.numInsert = 0;
    statistics.numDelete = 0;
    statistics.numLookUp = 0;
}

ThreadSafeKVStore :: ~ThreadSafeKVStore() {
    rwLock = PTHREAD_RWLOCK_INITIALIZER;
}

/**
 * This insert function is thread safe. A write lock is used
 * to protect the insert operation.
 * @ param key string to be inserted to the cache as key.
 * @ param value string to be inserted to the cacheas value.
 *   If key already exists, then change the value associated
 *   with that key to passed in value.
 * @ return an int value. If 0, the insert operation is successful.
 *   if -1, error occured during the insert operation.
 **/
int ThreadSafeKVStore :: insert(const string key, const string value) {
    try {
        pthread_rwlock_wrlock( & rwLock);
        auto it = myMap.find(key);
        if (it != myMap.end()) {
            cacheList.erase(it->second);
            myMap.erase(it);
        }
        cacheList.push_front(make_pair(key, value));
        myMap.insert(make_pair(key, cacheList.begin()));
        statistics.numInsert++;
        clean();
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
 * @ param key string to be looked up from the cache.                     
 * @ param value string to be overwritten if key is found
 *   and current value associated with the key is different.
 * @ return an int value. If 0, key is found.
 *   if -1, key is not found.
 **/
int ThreadSafeKVStore :: lookup(const string key, string& value){
    pthread_rwlock_rdlock( & rwLock);
    bool exist = true;
    auto it = myMap.find(key);
    if (it == myMap.end()) {
        exist = false;
    } else {
        value = it->second->second;
        cacheList.splice(cacheList.begin(), cacheList, it->second);
    }
    if (key == "STA") {
        value = "Number of insert operations on cache: " + to_string(statistics.numInsert) + "\nNumber of delete operations on cache: "
             + to_string(statistics.numDelete) + "\nNumber of lookup operations on cache: " + to_string(statistics.numLookUp);
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
 * @ param key string to be removed from the cache.
 * @ return an int value. If 0, the remove operation is successful.
 *   if -1, error occured during the remove operation.
 **/
int ThreadSafeKVStore :: remove(const string key){
    try {
        pthread_rwlock_wrlock( & rwLock);
        auto it = myMap.find(key);
        if (it != myMap.end()) {
            cacheList.erase(it->second);
            myMap.erase(it);
        }
        statistics.numDelete++;
        pthread_rwlock_unlock( & rwLock);
    } catch (const runtime_error& error) {
        return -1;
    }
    return 0;
}

/**
 * This function delete all least recently used elements from cacheList/hashmap once hashmap has a size 
 * larger than cache_size.
 **/
void ThreadSafeKVStore :: clean(void) {
    while (myMap.size() > cache_size) {
        auto last_it = cacheList.end();
        last_it--;
        myMap.erase(last_it->first);
        cacheList.pop_back();
    }
}
