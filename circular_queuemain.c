#include <stdio.h>
#include <stdlib.h>
#include "circular_queue.h"

int main()
{
    CircularQueue queue;
    initQueue(&queue);
    PCB *pcb1 = (PCB *)malloc(sizeof(PCB));
    pcb1->pri = 1;
    PCB *pcb2 = (PCB *)malloc(sizeof(PCB));
    pcb2->pri = 2;
    PCB *pcb3 = (PCB *)malloc(sizeof(PCB));
    pcb3->pri = 3;
    PCB *pcb4 = (PCB *)malloc(sizeof(PCB));
    pcb4->pri = 4;

    printf("Enqueueing elements:\n");
    enqueue(&queue, pcb1);
    printf("Enqueued: %d\n", pcb1->pri);
    enqueue(&queue, pcb2);
    printf("Enqueued: %d\n", pcb2->pri);
    enqueue(&queue, pcb3);
    printf("Enqueued: %d\n", pcb3->pri);
    enqueue(&queue, pcb4);
    printf("Enqueued: %d\n", pcb4->pri);
    displayQueue(&queue);

    printf("\nDequeuing elements:\n");
    for (int i = 0; i < 4; i++)
    {
        PCB *temp = (PCB *)malloc(sizeof(PCB *));
        if (dequeue(&queue, &temp))
        {
            printf("Dequeued: %d\n", temp->pri);
        }
        else
        {
            printf("Failed to dequeue (Queue is empty)\n");
        }
    }
    displayQueue(&queue);

    PCB *pcb5 = (PCB *)malloc(sizeof(PCB));
    pcb5->pri = 5;
    PCB *pcb6 = (PCB *)malloc(sizeof(PCB));
    pcb6->pri = 6;
    PCB *pcb7 = (PCB *)malloc(sizeof(PCB));
    pcb7->pri = 7;
    printf("\nEnqueueing more elements:\n");
    enqueue(&queue, pcb5);
    printf("Enqueued: %d\n", pcb5->pri);
    enqueue(&queue, pcb6);
    printf("Enqueued: %d\n", pcb6->pri);
    enqueue(&queue, pcb7);
    printf("Enqueued: %d\n", pcb7->pri);

    displayQueue(&queue);
    // Free the queue
    freeQueue(&queue);
    return 0;
}
