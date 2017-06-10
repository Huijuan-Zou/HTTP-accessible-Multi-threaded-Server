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

struct arg_struct {
    ThreadSafeKVStore arg1;
    unsigned long arg2;
};

struct clientInfo {
    int clientId;
    double startTime;
    double endTime;
};

static int listenSocket;
static struct sockaddr_in server;
static queue<clientInfo> clients;
static pthread_mutex_t printLock;
static pthread_mutex_t vectorLock;
std :: mutex queueLock;
static condition_variable cv;
static double maxRequestTime;
static double minRequestTime;
static double totalRequestTime;
vector<double> requestTimeArr;


ThreadPoolServer :: ThreadPoolServer() {
    printLock = PTHREAD_MUTEX_INITIALIZER;
    vectorLock = PTHREAD_MUTEX_INITIALIZER;
    maxRequestTime = numeric_limits<double>::min();
    minRequestTime = numeric_limits<double>::max();
    totalRequestTime = 0;
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
    server.sin_addr.s_addr = inet_addr("192.168.1.125 ");
    server.sin_port = portNumber;
    memset( & server.sin_zero, 0, sizeof(server.sin_zero));
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
        cout << "Binding error"<< endl;
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
        data.socket = accept(listenSocket, (struct sockaddr *) &data.info, &size);
        if ((int) data.socket < 0) {
            pthread_mutex_lock(& printLock);
            cout << "Error in accept client" << endl;
            pthread_mutex_unlock(& printLock);
        } else {
            try {
                unique_lock<mutex> lck(queueLock);
                clientInfo client;
                client.clientId = data.socket;
                client.startTime = clock();
                client.endTime = clock();
                clients.push(client);
                lck.unlock();
                cv.notify_one();
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
    vector<arg_struct> argsVec;
    for (unsigned long i = 0; i < numThreads; i++) {
        struct arg_struct args;
        args.arg1 = store;
        args.arg2 = i;
        argsVec.push_back(args);
    }
    for (unsigned int i = 0; i < numThreads; i++ ){
        if (pthread_create(& threads[i], NULL, executeRequest, (void*) &argsVec[i])){
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
void* ThreadPoolServer :: executeRequest(void* arguments) {
    struct arg_struct args = *(struct arg_struct*) arguments;
    ThreadSafeKVStore store = args.arg1;
    unsigned long threadID = args.arg2;

    while (true) {
        unique_lock<mutex> lck(queueLock);
        while (clients.empty()) {
            cv.wait(lck);
        }
        clientInfo client = clients.front();
        clients.pop();
        lck.unlock();
        if ( client.clientId >= 0) {
            pthread_mutex_lock(& printLock);
            cout << "Running client " << client.clientId << endl;
            pthread_mutex_unlock(& printLock);
            string request = "";
            bool finished = false;

            while (!finished) {
                char buffer[1000] = {0};
                int bytes = (int) recv(client.clientId, buffer, 1000, 0);
                if (bytes > 0) {
                  request.append(buffer, bytes);
                }
                if (bytes < 1000) {
                    finished = true;
                }
            }
            
            HttpRequestParser parser;
            requestInfo info = parser.requestParser(request);
            string response = responseBuilder(info, store);
            char* cstr = strdup(response.c_str());
            
            int byte_sent = (int) send(client.clientId, cstr, strlen(cstr), 0);
            delete [] cstr;
            client.endTime = clock();
            double requestTime = (client.endTime - client.startTime)/ (double) CLOCKS_PER_SEC;
            //calculate min, max, mean, medium
            pthread_mutex_lock(& vectorLock);
            if (requestTime > maxRequestTime) {
                maxRequestTime = requestTime;
            }
            if (requestTime < minRequestTime) {
                minRequestTime = requestTime;
            }
            if (requestTimeArr.empty()) {
                requestTimeArr.push_back(requestTime);
            } else {
                for (int i = 0; i < requestTimeArr.size(); i++) {
                if (requestTimeArr.at(i) > requestTime) {
                    requestTimeArr.insert(requestTimeArr.begin() + i, requestTime);
                    break;
                }
            }
        }
            
            totalRequestTime += requestTime;
            double curMax = maxRequestTime;
            double curMin = minRequestTime;
            double averageRequstTime = totalRequestTime / requestTimeArr.size();
            double curMedium = 0;
            int mid = (requestTimeArr.size() - 1) / 2;
            if (requestTimeArr.size() % 2 == 0 && mid >= 0) {
                curMedium = (requestTimeArr.at(mid) + requestTimeArr.at(mid + 1)) / 2;
            } else if (mid >= 0) {
                curMedium = requestTimeArr.at(mid);
            }
            pthread_mutex_unlock(& vectorLock);

            //get total number of delete, look up and insert
            string value = "a";
            store.lookup("STA", value);

            pthread_mutex_lock(& printLock);
            cout << "Request time of " << client.clientId <<  " on thread " << threadID <<
             " is " << requestTime * 1000 << "ms" << endl;
            cout << "Max so far is " << curMax * 1000 << "ms" << endl;
            cout << "Min so far is " << curMin * 1000 << "ms" <<  endl;
            cout << "Medium so far is " << curMedium * 1000 << "ms" << endl;
            cout << "Mean so far is " << averageRequstTime * 1000 << "ms" << endl;
            cout << value << endl;
            cout << endl;
            pthread_mutex_unlock(& printLock);
            shutdown(client.clientId, SHUT_WR);
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
            res = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        } else {
            res = "HTTP/1.1 200 OK\r\nContent-Length: " + to_string(value.length()) + "\r\n\r\n" + value;
        }
    } else if (info.type == POST) {
        store.insert(info.key, info.value);
        res = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
    } else if (info.type == DELETE) {
        string value = "a";
        if (store.lookup(info.key, value) != 0) {
            res = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        } else {
            store.remove(info.key);
            res = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
        }
    }
    return res;
}
