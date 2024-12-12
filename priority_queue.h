#include <headers.h>
#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H


typedef struct Node {
    PCB *pcb;
    struct Node* next;
} Node;

typedef struct PriorityQueue {
    Node* front;
} PriorityQueue;

// Function prototypes
PriorityQueue* createQueue();
void enqueue(PriorityQueue* pq, int value);
int dequeue(PriorityQueue* pq);
int isEmpty(PriorityQueue* pq);
void freeQueue(PriorityQueue* pq);

#endif // PRIORITY_QUEUE_H