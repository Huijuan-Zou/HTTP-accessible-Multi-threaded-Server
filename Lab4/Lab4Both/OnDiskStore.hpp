//
//  OnDiskStore.hpp
//  
//
//  Created by Huijuan Zou on 12/6/16.
//
//

#ifndef OnDiskStore_hpp
#define OnDiskStore_hpp

#include <stdio.h>
#include <fstream>
#include "ThreadSafeKVStore.hpp"

using namespace std;

struct cacheElement {
    pair<string, string> keyValue;
    int count;
};

class OnDiskStore {
    public :
    OnDiskStore();
    ~OnDiskStore();
    int insert(const string, const string, const string);
    int lookup(const string, string&, const string);
    int remove(const string, const string);
    
    private :
    pthread_rwlock_t rwLock = PTHREAD_RWLOCK_INITIALIZER;
};
#endif /* OnDiskStore_hpp */