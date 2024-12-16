#include "priority_queue.h"

// Create a new priority queue
PriorityQueue* createQueue() {
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    pq->front = NULL;
    return pq;
}

// Enqueue an element in ascending order
void enqueuePri(PriorityQueue* pq, PCB *pcb, int pri) {
    priNode* newNode = (priNode*)malloc(sizeof(priNode));
    newNode->pcb = pcb;
    newNode->next = NULL;
    newNode->priority = pri;

    // If the queue is empty or the new node has the highest priority (smallest value)
    if (pq->front == NULL || pq->front->priority > pri) {
        newNode->next = pq->front;
        pq->front = newNode;
    } else {
        // Find the correct position to insert the new node
        priNode* current = pq->front;
        while (current->next != NULL && current->next->priority <= pri) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

// Dequeue the element with the highest priority (smallest value)
bool dequeuePri(PriorityQueue* pq, PCB** retpcb) {
    if (isPriEmpty(pq)) {
        printf("Queue is empty!\n");
        return false;
    }
    priNode* temp = pq->front;

    // Return the pointer to the dequeued PCB
    *retpcb = temp->pcb;

    pq->front = pq->front->next;
    free(temp);
    return true;
}

// Check if the queue is empty
bool isPriEmpty(PriorityQueue* pq) {
    return pq->front == NULL;
}

// Free the memory allocated for the queue
void freePriQueue(PriorityQueue* pq) {
    while (!isPriEmpty(pq)) {
        PCB* tempPcb = NULL;
        dequeuePri(pq, &tempPcb);
        // PCB memory is not freed here because it should be managed by the caller
    }
    free(pq);
}

