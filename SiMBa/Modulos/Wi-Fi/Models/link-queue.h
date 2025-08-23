#ifndef LINK_QUEUE__H
#define LINK_QUEUE__H
#include "wi-fi-constants.h"

class LinkQueue {
private:
    static const int MAX_SIZE = 5;
    int queue[TCP_IP_MAX_LINKS];
    int front;
    int rear;
    int count;

public:
    LinkQueue();

    void Reset();

    bool IsEmpty() const;

    bool IsFull() const;

    bool Enqueue(int value);

    int Dequeue();

    int Peek() const;
};


#endif