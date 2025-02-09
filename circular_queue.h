#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H
#include "headers.h"

// Node structure for the queue
typedef struct Node {
    PCB* pcb;
    struct Node* next;
} Node;

// Circular queue structure
typedef struct {
    Node* rear;
} CircularQueue;

// Function declarations
void initQueue(CircularQueue* q);
bool isEmpty(CircularQueue* q);
void enqueue(CircularQueue* q, PCB* pcb);
bool dequeue(CircularQueue* q, PCB** retpcb);
void displayQueue(CircularQueue* q);
void freeQueue(CircularQueue* q);

#endif // CIRCULAR_QUEUE_H
