#include "headers.h"
// #include "priority_queue.h"
void RR(int q);
void MLFQ(int q);
void SJF();
void HPF();
void ProcessFinishedSJF(int signum);

PCB *current_process = NULL;

bool arrIsEmpty(CircularQueue* arr[]){
    bool empty=false;
    for(int i=0;i<sizeof(arr);i++){
        if(arr[i]->rear==NULL){
            empty=true;
        }
        else {
            empty=false;
            return empty;
        }
    }
    return true;
}

int main(int argc, char *argv[])
{
    initClk();
    printf("algo: %s, quan: %s \n", argv[1], argv[2]);
    // set up message queue between process generator and scheduler
    // key_t msg_id;
    // int send_val;
    // msg_id = ftok("msgfile", 65);
    // int msgq_id = msgget(msg_id, 0666 | IPC_CREAT);
    // if (msgq_id == -1)
    // {
    //     perror("Error in create");
    //     exit(-1);
    // }
    // compile the process file
    int process_compile = system("gcc process.c -o process.out");
    if (process_compile != 0)
    {
        printf("Couldn't compile process.c \n");
        exit(1);
    }
    // while(no more processes and ready queue is empty) still dk how we will implement it
    switch (atoi(argv[1]))
    {
    case 1:
        SJF();
        break;

    case 2:
        HPF();
        break;

    case 3:
        RR(atoi(argv[2]));
        break;

    case 4:
        MLFQ(atoi(argv[2]));
        break;
    }
    // the up coming loop is just for testing
    // ill leave the test loop lw 7ad 3yz yeshof setting up el msgq h comment it out bas
    //  struct msgbuff sentPCB;
    //  while(1){
    //      int rec_val = msgrcv(msgq_id, &sentPCB, sizeof(sentPCB.pcb), 1, !IPC_NOWAIT);
    //      if (rec_val != 1){
    //          printf("Received process in scheduler %d at time %d with runtime %d and priority %d \n",sentPCB.pcb.id,getClk(),sentPCB.pcb.runtime,sentPCB.pcb.priority);
    //      }
    //  }
    // TODO: implement the scheduler.
    // TODO: upon termination release the clock resources.

    destroyClk(true);
    return 0;
}

void RR(int q)
{
}

void MLFQ(int q){
bool readyqEmpty=false;
CircularQueue* queuearray[11]; //array holding queues for each priority level 
//for loop to create 11 queues (pri levels 0 to 10)
for (int i=0;i<11;i++){
    CircularQueue *q=malloc(sizeof(CircularQueue));
    initQueue(q);
    queuearray[i]=q;
}

struct msgbuff sentPCB; //struct to recieve pcb from pgen
while(!readyqEmpty){
    readyqEmpty=!arrIsEmpty(queuearray);
    int rcv_val= msgrcv(msgq_id,&sentPCB,sizeof(sentPCB.pcb),1,IPC_NOWAIT);
    if(rcv_val!=-1){
        //new pcb recieved, place it in correct queue
        int pri=sentPCB.pcb.priority;
        enqueue(queuearray[pri],&sentPCB);
    }

}
}

void SJF()
{
    // attach handler of finished processes
    signal(SIGUSR1, ProcessFinishedSJF);
    // set up message queue between process generator and schedular
    key_t msg_id;
    int send_val;
    msg_id = ftok("msgfile", 65);
    int msgq_id = msgget(msg_id, 0666 | IPC_CREAT);
    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    // //the up coming loop is just for testing
    // struct msgbuff sentPCB;
    // int rec_val = msgrcv(msgq_id, &sentPCB, sizeof(sentPCB.pcb), 1, !IPC_NOWAIT);
    // if (rec_val != 1){
    //     printf("Received process in scheduler %d at time %d with runtime %d and priority %d \n",sentPCB.pcb.id,getClk(),sentPCB.pcb.runtime,sentPCB.pcb.priority);
    // }

    // create a ready priority queue
    PriorityQueue *ReadyQueue = createQueue();
    struct msgbuff receivedPCBbuff;
    printf("entering sjf\n");
    // wait for first process to arrive to start the algo
    int rec_val = msgrcv(msgq_id, &receivedPCBbuff, sizeof(receivedPCBbuff.pcb), 1, !IPC_NOWAIT);
    // if there is a process sent add it in the ready queue
    if (rec_val != 1)
    {
        PCB *receivedPCB = malloc(sizeof(PCB));
        memcpy(receivedPCB, &receivedPCBbuff.pcb, sizeof(PCB));
        enqueuePri(ReadyQueue, receivedPCB, receivedPCB->runtime);
        printf("Received process in scheduler %d at time %d with runtime %d and priority %d \n", receivedPCB->id, getClk(), receivedPCB->runtime,receivedPCB->priority);
    }
    int mq_open = 1;
    // loop while there is still processes unfinished or the process generator didn't close the message queue
    while (!isPriEmpty(ReadyQueue) || mq_open || current_process != NULL)
    {
        int rec_val = msgrcv(msgq_id, &receivedPCBbuff, sizeof(receivedPCBbuff.pcb), 1, IPC_NOWAIT);
        if (rec_val == -1) {
            if (errno == ENOMSG) {
                // No message in the queue 
                errno = 0; // Reset errno to avoid stale values
            } else {
                mq_open = 0; //the process generator has closed the message queue
            }
        }
        // if there is a process sent add it in the ready queue
        if (rec_val != -1)
        {
            PCB *receivedPCB = malloc(sizeof(PCB));
            memcpy(receivedPCB, &receivedPCBbuff.pcb, sizeof(PCB));
            // receivedPCB->id = receivedPCBbuff.pcb.id;
            // receivedPCB->arrival_time = receivedPCBbuff.pcb.arrival_time;
            // receivedPCB->runtime = receivedPCBbuff.pcb.runtime;
            // receivedPCB->priority = receivedPCBbuff.pcb.priority;
            // receivedPCB->pid = -1;
            // receivedPCB->remaining_time = receivedPCBbuff.pcb.remaining_time;
            // receivedPCB->waiting_time = 0;
            // receivedPCB->state = READY;
            enqueuePri(ReadyQueue, receivedPCB, receivedPCB->runtime);
            printf("Received process in scheduler %d at time %d with runtime %d and priority %d \n", receivedPCB->id, getClk(), receivedPCB->runtime,receivedPCB->priority);
        }
        // if there is no process running and there is a ready process
        if (current_process == NULL && !isPriEmpty(ReadyQueue))
        {
            dequeuePri(ReadyQueue, &current_process);
            // set its status to running
            current_process->state = RUNNING;
            // fork the process and run it
            int current_processID = fork();
            if (current_processID == 0)
            {
                // run it and send the process the scheduler's id
                char id[20];
                sprintf(id, "%d", getppid());
                char runtime[20];
                sprintf(runtime, "%d", current_process->runtime);
                execl("./process.out", "process.out",runtime ,id, NULL);
                printf("error in excel of process\n");
            }
            printf("Process: %d : \n",current_process->id);
            current_process->pid = current_processID;
        }
    }
    printf("Done\n");
}

void HPF()
{
}

void ProcessFinishedSJF(int signum)
{
    printf("Process %d finished at time %d \n",current_process->id,getClk());
    // if the process sends SIGUSR1 then the current process finished
    current_process->state = FINISHED;
    free(current_process);
    // set the current process to null
    current_process = NULL;
}