#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include <stdbool.h>

// Node structure for the queue
typedef struct Node {
    int data;
    struct Node* next;
} Node;

// Circular queue structure
typedef struct {
    Node* rear;
} CircularQueue;

// Function declarations
void initQueue(CircularQueue* q);
bool isEmpty(CircularQueue* q);
void enqueue(CircularQueue* q, int value);
bool dequeue(CircularQueue* q, int* value);
void displayQueue(CircularQueue* q);
void freeQueue(CircularQueue* q);

#endif // CIRCULAR_QUEUE_H
