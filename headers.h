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
#include <errno.h>
#include <string.h>

typedef short bool;
#define true 1
#define false 0

#define SHKEY 300

typedef struct PCB
{
    int id; // read from file
    int arrival_time;
    int runtime;
    int priority;
    int state;
    int remaining_time;
    int waiting_time;
    int pid; // forking id
    int start_time;
    int finished_time;
    int stopped_time;
    int restarted_time;
    int remainingTimeAfterStop;
    int memsize;
    int start_address;
    int end_address;
} PCB;

enum STATE
{
    READY,    // 0
    RUNNING,  // 1
    FINISHED, // 2
    STARTED,  // 3
    RESUMED,  // 4
    STOPPED   // 5
};

const char *stateStrings[] = {"READY", "RUNNING", "FINISHED", "STARTED", "RESUMED", "STOPPED"};

typedef struct msgbuff
{
    long mtype;
    PCB pcb;
} msgbuff;

///==============================
// don't mess with this variable//
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
        // Make sure that the clock exists
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
typedef struct Node
{
    PCB *pcb;
    struct Node *next;
} Node;

// Circular queue structure
typedef struct
{
    Node *rear;
} CircularQueue;

// Initialize the circular queue
void initQueue(CircularQueue *q)
{
    q->rear = NULL;
}

// Check if the queue is empty
bool isEmpty(CircularQueue *q)
{
    return q->rear == NULL;
}

// Enqueue an element into the queue
void enqueue(CircularQueue *q, PCB *pcb)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (!newNode)
    {
        printf("Memory allocation failed\n");
        return;
    }
    if (pcb != NULL)
    {
        newNode->pcb = pcb;

        if (isEmpty(q))
        {
            newNode->next = newNode; // Points to itself
            q->rear = newNode;
        }
        else
        {
            newNode->next = q->rear->next;
            q->rear->next = newNode;
            q->rear = newNode;
        }
    }
    else
    {
        printf("PCB is NULL \n");
    }
}

// Dequeue an element from the queue
bool dequeue(CircularQueue *q, PCB **retpcb)
{
    if (isEmpty(q))
    {
        return false; // Queue is empty
    }

    Node *temp = q->rear->next;
    *retpcb = temp->pcb;

    if (q->rear == temp)
    {
        q->rear = NULL; // Queue is now empty
    }
    else
    {
        q->rear->next = temp->next;
    }

    free(temp);
    return true;
}

void peak(CircularQueue *q, PCB **retpcb)
{
    Node *temp = q->rear->next;
    *retpcb = temp->pcb;
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
void freeQueue(CircularQueue *q)
{
    if (isEmpty(q))
    {
        return;
    }

    Node *current = q->rear->next;
    Node *nextNode;
    while (current != q->rear)
    {
        nextNode = current->next;
        free(current);
        current = nextNode;
    }
    free(q->rear);
    q->rear = NULL;
}

typedef struct priNode
{
    PCB *pcb;
    struct priNode *next;
    int priority;
} priNode;

typedef struct PriorityQueue
{
    priNode *front;
} PriorityQueue;

// Create a new priority queue
PriorityQueue *createQueue()
{
    PriorityQueue *pq = (PriorityQueue *)malloc(sizeof(PriorityQueue));
    pq->front = NULL;
    return pq;
}

// Check if the queue is empty
bool isPriEmpty(PriorityQueue *pq)
{
    return pq->front == NULL;
}

// Enqueue an element in ascending order
void enqueuePri(PriorityQueue *pq, PCB *pcb, int pri)
{
    priNode *newNode = (priNode *)malloc(sizeof(priNode));
    newNode->pcb = pcb;
    newNode->next = NULL;
    newNode->priority = pri;

    // If the queue is empty or the new node has the highest priority (smallest value)
    if (pq->front == NULL || pq->front->priority > pri)
    {
        newNode->next = pq->front;
        pq->front = newNode;
    }
    else
    {
        // Find the correct position to insert the new node
        priNode *current = pq->front;
        while (current->next != NULL && current->next->priority <= pri)
        {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

// Dequeue the element with the highest priority (smallest value)
bool dequeuePri(PriorityQueue *pq, PCB **retpcb)
{
    if (isPriEmpty(pq))
    {
        printf("Queue is empty!\n");
        return false;
    }
    priNode *temp = pq->front;

    // Return the pointer to the dequeued PCB
    *retpcb = temp->pcb;

    pq->front = pq->front->next;
    free(temp);
    return true;
}

// Free the memory allocated for the queue
void freePriQueue(PriorityQueue *pq)
{
    while (!isPriEmpty(pq))
    {
        PCB *tempPcb = NULL;
        dequeuePri(pq, &tempPcb);
        // PCB memory is not freed here because it should be managed by the caller
    }
    free(pq);
}

typedef struct BuddyMemory
{
    int memsize;
    int start;
    bool is_free;
    int pcbID;
    struct BuddyMemory *left;
    struct BuddyMemory *right;
} BuddyMemory;

// typedef struct MemoryHead
// {
//     struct BuddyMemory *root;
// } MemoryHead;

bool allocate(BuddyMemory *head, PCB *pcb)
{
    if (!pcb){
        return false;
    }
    if (!head)
        return false;
    if (!head->is_free)
        return false;
    if (head->memsize < pcb->memsize)
        return false;
    if (head->memsize == pcb->memsize && !head->left)
    {
        head->pcbID = pcb->id;
        pcb->start_address = head->start;
        pcb->end_address = head->start + head->memsize - 1;
        head->is_free = false;
        return true;
    }
    if (head->memsize >= pcb->memsize)
    {
        if (head->memsize / 2 < pcb->memsize && !head->left)
        {
            head->pcbID = pcb->id;
            pcb->start_address = head->start;
            pcb->end_address = head->start + head->memsize - 1;
            head->is_free = false;
            return true;
        }
        if (!head->left)
        {
            BuddyMemory *left = malloc(sizeof(BuddyMemory));
            BuddyMemory *right = malloc(sizeof(BuddyMemory));
            head->left = left;
            head->left->start = head->start;
            head->left->memsize = head->memsize / 2;
            head->left->is_free = true;
            head->right = right;
            head->right->memsize = head->memsize / 2;
            head->right->start = head->start + head->memsize / 2;
            head->right->is_free = true;
        }
        bool leftAllocate = allocate(head->left,pcb);
        bool rightAllocate = false;
        if (!leftAllocate){
            rightAllocate = allocate(head->right,pcb);
        }
        return leftAllocate || rightAllocate;
    }
}

void deallocate(BuddyMemory *head, int pcbStart)
{
    if (!head)
        return;
    if (head->is_free)
        return;
    if (!head->is_free && head->start == pcbStart)
    {
        head->is_free = true;
        head->pcbID = -1;
        free(head);
        return;
    }
    if (!head->left || !head->right)
        return;
    deallocate(head->left, pcbStart);
    deallocate(head->right, pcbStart);
    if (head->left && head->right && head->left->is_free && head->right->is_free)
    {
        if (!head->left->right && !head->left->left && !head->right->left && !head->right->right)
        {
            head->left = NULL;
            head->right = NULL;
        }
    }
}
void displayTree(BuddyMemory *head, int depth, const char *position)
{
    if (!head)
    {
        return;
    }

    // Indent based on depth to visualize tree levels
    for (int i = 0; i < depth; i++)
    {
        printf("  ");
    }

    // Display whether the block is "Left" or "Right" or "Root"
    if (position)
    {
        printf("[%s] ", position);
    }

    // Print information about the block
    printf("Block: Start=%d, Size=%d, Free=%s",
           head->start, head->memsize, head->is_free ? "Yes" : "No");

    // If allocated, display the PCB ID and memory range
    if (!head->is_free && head->pcbID != -1)
    {
        printf(", Allocated to PCB ID=%d (Start=%d, End=%d)",
               head->pcbID, head->start, head->start + head->memsize - 1);
    }

    printf("\n");

    // Recursively display left and right children
    displayTree(head->left, depth + 1, "Left");
    displayTree(head->right, depth + 1, "Right");
}

