//
//  ThreadPoolServer.cpp
//  Lab2
//
//  Created by Huijuan Zou on 10/29/16.
//  Copyright © 2016 Huijuan Zou. All rights reserved.
//

#include "ThreadPoolServer.hpp"

struct dataSocket {
    int socket;
    struct sockaddr_in info;
};

static int listenSocket;
static struct sockaddr_in server;
static ThreadSafeQueue clients;

ThreadPoolServer :: ThreadPoolServer() {
}

ThreadPoolServer :: ~ThreadPoolServer() {
}

/**
 * function to create listening socket.
 * @return bool value whether the creating socket is sucessful.
 **/
bool ThreadPoolServer :: creat_Socket(int portNumber) {
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket < 0) {
        return false;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = portNumber;
    //memset( & server.sin_zero, 0, sizeof(server.sin_zero));
    return true;
}

/**
 * function to bind listen socket and server.
 * @return bool value whether the binding is sucessful.
 **/
bool ThreadPoolServer :: binding() {
    try {
        :: bind(listenSocket, (sockaddr *)& server, (socklen_t) sizeof(server));
        listen(listenSocket, 5);
    } catch (const runtime_error& error) {
        cout << "Binding error creating"<< endl;
    }
    return true;
}

/**
 * This function starts the server. The server will keep running and wait for connections from clients.
 * @param argument number of threads to be emitted to work on processing http requests. .
 **/
void* ThreadPoolServer :: runServer(void * argument) {
    int portNumber = *(int*) argument;
    if (!creat_Socket(portNumber)) {
        throw string("Error in creating socket.");
    }
    if (!binding()) {
        throw string("Error in binding.");
    }
    while (true) {
        dataSocket data;
        socklen_t size = sizeof(data.info);
        data.socket = accept(listenSocket, (sockaddr *) &data.info, &size);
        if ((int) data.socket < 0) {
            cout << "Error in accept client" << endl;
        } else {
            try {
               clients.push(data.socket);
            } catch (const runtime_error& error) {
                cout << "Error in adding client" << endl;
            }
        }
    }
}

/**
 * This function starts thread pool to work on connections.
 * @param argument number of threads to be emitted to work on processing http requests.
 **/
void* ThreadPoolServer :: runThreadPool(void* argument) {
    int numThreads = *(int*) argument;
    ThreadSafeKVStore store;
    pthread_t threads[numThreads];
    vector<ThreadSafeKVStore> stores;
    for (unsigned int i = 0; i < numThreads; i++ ) {
        stores.push_back(store);
    }
    for (unsigned int i = 0; i < numThreads; i++ ){
        if (pthread_create(& threads[i], NULL, executeRequest, (void*) &stores[i])){
            cout << "Oh-no, error in creating thread " << i << endl;
        }
        pthread_detach(threads[i]);
    }
    pthread_exit(NULL);
}


/**
 * This is the entrance of the ThreadPoolServer class. Other functions need to call this function to
 * start the server.
 * @param portNumber port Number of listen socket.
 * @param numThreads total number of threads to be emitted to work on processing http requests.
 **/
void ThreadPoolServer :: run (int portNumber, int numThreads){
     pthread_t thread[2];
    if (pthread_create(& thread[0], NULL, ThreadPoolServer :: runThreadPool, (void*) &numThreads)){
        cout << "Oh-no, error in creating thread " << 0 << endl;
    }
    if (pthread_create(& thread[1], NULL, ThreadPoolServer :: runServer, (void*) &portNumber)){
        cout << "Oh-no, error in creating thread " << 1 << endl;
    }
    for (int i = 0; i < 2; i++) {
        pthread_join(thread[i], NULL);
    }
}

/**
 * This function is a next stage of runThreadPool, threads start to run on their own and process the requests.
 * Threads will keep running and keep pop out connection (client) from workqueue (clients). They wil only 
 * start work on request when pop() indicates the client is not negative (negative means work queue is empty).
 * @param argument Thread safe hash map instance, to be used for processing parsed request.
 **/
void* ThreadPoolServer :: executeRequest(void* argument) {
    ThreadSafeKVStore store = *(ThreadSafeKVStore*) argument;
    while (true) {
        int client = clients.pop();
        if ( client >= 0) {
            string request = "";
            while (true) {
                char buffer[10] = {0};
                int bytes = (int) recv(client, buffer, 10, 0);
                if (bytes > 0) {
                  request.append(buffer, bytes);
                }
                if (bytes < 10) {
                    break;
                }
            }
            HttpRequestParser parser;
            requestInfo info = parser.requestParser(request);
            string response = responseBuilder(info, store);
            send(client, response.c_str(), response.length(), 0);
        }
    }
    return NULL;
}

/**
 * @param info Parsed request information to be processed.
 * @param store Thread safe hash map instance, used for processing parsed request.
 * @return http/1.1 standard response
 **/
string ThreadPoolServer :: responseBuilder(requestInfo &info, ThreadSafeKVStore &store) {
    string res = "";
    string status;
    string contentLength;
    
    if (info.type == GET) {
        string value = "a";
        int find = store.lookup(info.key, value);
        if (find != 0) {
            res = "HTTP/1.1 404 Not Found\nContent-Length: 0\n";
        } else {
            res = "HTTP/1.1 200 OK\nContent-Length: " + to_string(value.length()) + "\n\n" + value;
        }
    } else if (info.type == POST) {
        store.insert(info.key, info.value);
        res = "HTTP/1.1 200 OK\nContent-Length: 0\n";
    } else if (info.type == DELETE) {
        string value = "a";
        if (store.lookup(info.key, value) != 0) {
            res = "HTTP/1.1 404 Not Found\nContent-Length: 0\n";
        } else {
            store.remove(info.key);
            res = "HTTP/1.1 200 OK\nContent-Length: 0\n";
        }
    }
    return res;
}
