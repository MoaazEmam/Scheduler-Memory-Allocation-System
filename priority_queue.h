#include "headers.h"
#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H


typedef struct Node {
    PCB *pcb;
    struct Node* next;
    int priority;
} Node;

typedef struct PriorityQueue {
    Node* front;
} PriorityQueue;

// Function prototypes
PriorityQueue* createQueue();
void enqueue(PriorityQueue* pq,PCB *pcb,int pri);
bool dequeue(PriorityQueue* pq,PCB **retpcb);
bool isEmpty(PriorityQueue* pq);
void freeQueue(PriorityQueue* pq);

#endif // PRIORITY_QUEUE_H