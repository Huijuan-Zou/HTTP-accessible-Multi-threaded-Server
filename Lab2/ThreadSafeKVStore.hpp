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

using namespace std;

class ThreadSafeKVStore {
    public :
    ThreadSafeKVStore();
    ~ThreadSafeKVStore();
    int insert(const string, const string);
    int lookup(const string, string&);
    int remove(const string);
    
    private :
    unordered_map<string, string> myStore;
    pthread_rwlock_t rwLock = PTHREAD_RWLOCK_INITIALIZER;
};

#endif /* ThreadSafeKVStore_hpp */

