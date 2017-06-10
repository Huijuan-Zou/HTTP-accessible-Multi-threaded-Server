#ifndef ThreadSafeQueue_hpp
#define ThreadSafeQueue_hpp

#include <stdio.h>
#include <queue>
#include <mutex>

using namespace std;

class ThreadSafeQueue {
    public :
    ThreadSafeQueue();
    ~ThreadSafeQueue();
    void push(const int  value);
    int pop();
    bool empty();
private:
    queue<int> myQueue;
    mutex myLock;
};
#endif /* ThreadSafeQ_hpp */


