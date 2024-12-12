#include <stdio.h>
#include <stdlib.h>
#include "circular_queue.h"

// Initialize the circular queue
void initQueue(CircularQueue* q) {
    q->rear = NULL;
}

// Check if the queue is empty
bool isEmpty(CircularQueue* q) {
    return q->rear == NULL;
}

// Enqueue an element into the queue
void enqueue(CircularQueue* q, int value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        printf("Memory allocation failed\n");
        return;
    }
    newNode->data = value;

    if (isEmpty(q)) {
        newNode->next = newNode; // Points to itself
        q->rear = newNode;
    } else {
        newNode->next = q->rear->next;
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

// Dequeue an element from the queue
bool dequeue(CircularQueue* q, int* value) {
    if (isEmpty(q)) {
        return false; // Queue is empty
    }

    Node* temp = q->rear->next;
    *value = temp->data;

    if (q->rear == temp) {
        q->rear = NULL; // Queue is now empty
    } else {
        q->rear->next = temp->next;
    }

    free(temp);
    return true;
}

// Display the elements of the queue
void displayQueue(CircularQueue* q) {
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return;
    }

    Node* temp = q->rear->next;
    printf("Queue elements: ");
    do {
        printf("%d ", temp->data);
        temp = temp->next;
    } while (temp != q->rear->next);
    printf("\n");
}

// Free all nodes in the queue
void freeQueue(CircularQueue* q) {
    if (isEmpty(q)) {
        return;
    }

    Node* current = q->rear->next;
    Node* nextNode;
    while (current != q->rear) {
        nextNode = current->next;
        free(current);
        current = nextNode;
    }
    free(q->rear);
    q->rear = NULL;
}
