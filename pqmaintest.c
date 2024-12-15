#include "pq.h"

int main() {
    // Create a new priority queue
    PriorityQueue* pq = createQueue();

    // Allocate memory for PCBs
    PCB *pcb1 = (PCB*)malloc(sizeof(PCB));
    pcb1->pri = 1;
    PCB *pcb2 = (PCB*)malloc(sizeof(PCB));
    pcb2->pri = 2;
    PCB *pcb3 = (PCB*)malloc(sizeof(PCB));
    pcb3->pri = 3;
    PCB *pcb4 = (PCB*)malloc(sizeof(PCB));
    pcb4->pri = 4;

    // Enqueue the PCBs
    enqueue(pq, pcb1, 2);
    enqueue(pq, pcb2, 1);
    enqueue(pq, pcb3, 2);
    enqueue(pq, pcb4, 3);

    // Dequeue and print the PCB priorities
    PCB* temp = NULL;
    if (dequeue(pq, &temp)) {
        printf("Dequeued PCB priority: %d\n", temp->pri);
    }
    if (dequeue(pq, &temp)) {
        printf("Dequeued PCB priority: %d\n", temp->pri);
    }
    if (dequeue(pq, &temp)) {
        printf("Dequeued PCB priority: %d\n", temp->pri);
    }
    if (dequeue(pq, &temp)) {
        printf("Dequeued PCB priority: %d\n", temp->pri);
    }
    if (dequeue(pq, &temp)) {
        printf("Dequeued PCB priority: %d\n", temp->pri);
    }

    // Free allocated memory for PCBs
    free(pcb1);
    free(pcb2);
    free(pcb3);
    free(pcb4);

    // Free the queue memory
    freeQueue(pq);

    return 0;
}
