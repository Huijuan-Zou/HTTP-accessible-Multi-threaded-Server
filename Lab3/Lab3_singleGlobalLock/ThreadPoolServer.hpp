//
//  ThreadPoolServer.hpp
//  Lab2
//
//  Created by Huijuan Zou on 10/29/16.
//  Copyright © 2016 Huijuan Zou. All rights reserved.
//

#ifndef ThreadPoolServer_hpp
#define ThreadPoolServer_hpp

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <queue>
#include <condition_variable>

#include "ThreadSafeKVStore.hpp"
#include "HttpRequestParser.hpp"


class ThreadPoolServer {
public:
    ThreadPoolServer();
    ~ThreadPoolServer();
    void run(int, int);
    
private:
    static bool creat_Socket(int portNumber);
    static bool binding();
    static void* runServer(void* );
    static void* runThreadPool(void*);
    static void* executeRequest(void* arguments);
    static string responseBuilder(requestInfo &, ThreadSafeKVStore &);
};
#endif /* ThreadPoolServer_hpp */
