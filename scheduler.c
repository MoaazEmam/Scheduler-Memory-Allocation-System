#include "headers.h"
//#include "priority_queue.h"
void RR(int q);
void MLFQ(int q);
void SJF();
void HPF();
void ProcessFinishedSJF(int signum);

PCB *current_process = NULL;

int main(int argc, char *argv[])
{
    initClk();
    printf("algo: %s, quan: %s \n", argv[1], argv[2]);
    // set up message queue between process generator and scheduler
    key_t msg_id;
    int send_val;
    msg_id = ftok("msgfile", 65);
    int msgq_id = msgget(msg_id, 0666 | IPC_CREAT);
    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
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
}

void RR(int q)
{
}

void MLFQ(int q)
{
}

void SJF()
{
    //attach handler of finished processes
    signal(SIGUSR1, ProcessFinishedSJF);
    // set up message queue between process generator and scheduler
    key_t msg_id;
    int send_val;
    msg_id = ftok("msgfile", 65);
    int msgq_id = msgget(msg_id, 0666 | IPC_CREAT);
    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    // create a ready priority queue
    PriorityQueue *ReadyQueue = createQueue();
    struct msgbuff receivedPCBbuff;
    // loop while there is still processes unfinished and the process generator didn't close the message queue
    while (!isPriEmpty(ReadyQueue) || errno != EIDRM)
    {
        int rec_val = msgrcv(msgq_id, &receivedPCBbuff, sizeof(receivedPCBbuff.pcb), 1, IPC_NOWAIT);
        // if there is a process sent add it in the ready queue
        if (rec_val != -1)
        {
            PCB *receivedPCB = malloc(sizeof(PCB));
            memcpy(receivedPCB, &receivedPCBbuff.pcb, sizeof(PCB));
            enqueuePri(ReadyQueue, receivedPCB, receivedPCB->runtime);
        }
        // if there is no process running and there is a ready process
        if (current_process == NULL && !isPriEmpty(ReadyQueue))
        {
            dequeuePri(ReadyQueue, &current_process);
            // set its status to running
            current_process->state = RUNNING;
            //fork the process and run it 
            int current_processID = fork();
            if (current_processID == 0)
            {
                //run it and send the process the scheduler's id
                execl("./process.out", "process.out",getppid(),NULL);
                printf("error in excel of process\n");
            }
            current_process->pid = current_processID;
        }
    }
}

void HPF()
{
}

void ProcessFinishedSJF(int signum){
    //if the process sends SIGUSR1 then the current process finished
    current_process->state = FINISHED;
    //set the current process to null
    current_process = NULL;
}