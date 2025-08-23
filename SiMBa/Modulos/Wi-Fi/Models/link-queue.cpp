#include "link-queue.h"

LinkQueue::LinkQueue() : front(0), rear(0), count(0) {}

bool LinkQueue::IsEmpty() const {
    return count == 0;
}

bool LinkQueue::IsFull() const {
    return count == TCP_IP_MAX_LINKS;
}

bool LinkQueue::Enqueue(int value) {
    if (IsFull()) return false;
    queue[rear] = value;
    rear = (rear + 1) % TCP_IP_MAX_LINKS;
    count++;
    return true;
}

int LinkQueue::Dequeue() {
    if (IsEmpty()) return -1; 
    int value = queue[front];
    front = (front + 1) % TCP_IP_MAX_LINKS;
    count--;
    return value;
}

int LinkQueue::Peek() const {
    if (IsEmpty()) return -1;
    return queue[front];
}

void LinkQueue::Reset(){
    front =0;
    rear = 0;
    count =0;
}

