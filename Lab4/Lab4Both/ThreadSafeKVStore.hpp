//
//  ThreadSafeKVStore.hpp
//  Lab2
//
//  Created by Huijuan Zou on 10/29/16.
//  Copyright © 2016 Huijuan Zou. All rights reserved.
//

#ifndef ThreadSafeKVStore_hpp
#define ThreadSafeKVStore_hpp

#include <stdio.h>
#include <string>
#include <unordered_map>
#include <pthread.h>
#include <list>

using namespace std;

class ThreadSafeKVStore {
    public :
    ThreadSafeKVStore();
    ~ThreadSafeKVStore();
    int insert(const string, const string);
    int lookup(const string, string&);
    int remove(const string);
    
    private :
    pthread_rwlock_t rwLock;
    list<pair<string, string>> cacheList;
    unordered_map<string, decltype(cacheList.begin())> myMap;
    size_t cache_size = 128;
    void clean(void);
};

#endif /* ThreadSafeKVStore_hpp */

