#include <stdio.h>
#include <stdlib.h>
#include "priority_queue.h"

// Create a new priority queue
PriorityQueue* createQueue() {
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    pq->front = NULL;
    return pq;
}

// Enqueue an element in ascending order
void enqueue(PriorityQueue* pq, PCB *pcb,int pri) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->pcb = pcb;
    newNode->next = NULL;

    // If the queue is empty or the new node has the highest priority (smallest value)
    if (pq->front == NULL || pq->front->data > pri) {
        newNode->next = pq->front;
        pq->front = newNode;
    } else {
        // Find the correct position to insert the new node
        Node* current = pq->front;
        while (current->next != NULL && current->next->data <= pri) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

// Dequeue the element with the highest priority (smallest value)
int dequeue(PriorityQueue* pq) {
    if (isEmpty(pq)) {
        printf("Queue is empty!\n");
        return -1; // Indicate that the queue is empty
    }
    Node* temp = pq->front;
    int value = temp->data;
    pq->front = pq->front->next;
    free(temp);
    return value;
}

// Check if the queue is empty
int isEmpty(PriorityQueue* pq) {
    return pq->front == NULL;
}

// Free the memory allocated for the queue
void freeQueue(PriorityQueue* pq) {
    while (!isEmpty(pq)) {
        dequeue(pq);
    }
    free(pq);
}