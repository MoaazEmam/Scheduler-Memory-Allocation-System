#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

typedef short bool;
#define true 1
#define false 0

#define SHKEY 300

typedef struct PCB {
   int id; //read from file
   int arrival_time;
   int runtime;
   int priority;
   int state; //1.Running 2.Ready 3.Blocked => enum lesa
   int remaining_time;
   int waiting_time;
   int pid; //forking id
} PCB;

enum STATE{
    READY, //0
    RUNNING, //1
    FINISHED, //2
    STARTED, //3
    RESUMED, //4
    STOPPED //5
};

typedef struct msgbuff
{
    long mtype;
    PCB pcb;
} msgbuff;

///==============================
//don't mess with this variable//
int *shmaddr; //
//===============================

int getClk()
{
    return *shmaddr;
}

/*
 * All processes call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

/*
 * All processes call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}

// Node structure for the queue
typedef struct Node {
    PCB* pcb;
    struct Node* next;
} Node;

// Circular queue structure
typedef struct {
    Node* rear;
} CircularQueue;

// Initialize the circular queue
void initQueue(CircularQueue* q) {
    q->rear = NULL;
}

// Check if the queue is empty
bool isEmpty(CircularQueue* q) {
    return q->rear == NULL;
}

// Enqueue an element into the queue
void enqueue(CircularQueue* q, PCB* pcb) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        printf("Memory allocation failed\n");
        return;
    }
    newNode->pcb = pcb;

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
bool dequeue(CircularQueue* q, PCB** retpcb) {
    if (isEmpty(q)) {
        return false; // Queue is empty
    }

    Node* temp = q->rear->next;
    *retpcb = temp->pcb;

    if (q->rear == temp) {
        q->rear = NULL; // Queue is now empty
    } else {
        q->rear->next = temp->next;
    }

    free(temp);
    return true;
}

// Display the elements of the queue
// void displayQueue(CircularQueue* q) {
//     if (isEmpty(q)) {
//         printf("Queue is empty\n");
//         return;
//     }

//     Node* temp = q->rear->next;
//     printf("Queue elements: ");
//     do {
//         printf("%d ", temp->pcb->pri);
//         temp = temp->next;
//     } while (temp != q->rear->next);
//     printf("\n");
// }

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
