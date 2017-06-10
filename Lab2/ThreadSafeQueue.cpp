//
//  ThreadSafeQueue.cpp
//  Lab2
//
//  Created by Huijuan Zou on 11/3/16.
//  Copyright © 2016 Huijuan Zou. All rights reserved.
//

#include "ThreadSafeQueue.hpp"

ThreadSafeQueue ::ThreadSafeQueue() {
}

ThreadSafeQueue ::~ThreadSafeQueue() {
}

/**
 * function to push element to queue.
 **/
void ThreadSafeQueue :: push(const int value) {
    lock_guard<mutex> lock(myLock);
    myQueue.push(value);
}

/**
 * function to remove front element from queue.
 * @return the element being removed.
 **/
int ThreadSafeQueue :: pop() {
    lock_guard<mutex> lock(myLock);
    if (!myQueue.empty()) {
        int front = myQueue.front();
        myQueue.pop();
        return front;
    }
    return -1;
}