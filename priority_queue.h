#include "headers.h"
#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H


typedef struct priNode {
    PCB *pcb;
    struct priNode* next;
    int priority;
} priNode;

typedef struct PriorityQueue {
    priNode* front;
} PriorityQueue;

// Function prototypes
PriorityQueue* createQueue();
void enqueuePri(PriorityQueue* pq,PCB *pcb,int pri);
bool dequeuePri(PriorityQueue* pq,PCB **retpcb);
bool isPriEmpty(PriorityQueue* pq);
void freePriQueue(PriorityQueue* pq);

#endif // PRIORITY_QUEUE_H