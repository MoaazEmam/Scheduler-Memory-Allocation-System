#include "headers.h"
// #include "priority_queue.h"
void RR(int q);
void MLFQ(int q);
void SJF();
void HPF();
void ProcessFinishedSJF(int signum);
void ProcessFinishedRR(int signum);

FILE *pFile;
float WTA_sum = 0;

PCB *current_process = NULL;

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
    signal(SIGUSR1, ProcessFinishedRR);
    //open log file
     pFile = fopen("scheduler.log", "w");
     fprintf(pFile,"#At time x process y state arr w total z remain y wait k \n");
     fflush(pFile);
    // create msgq to receive
    int send_val;
    key_t msg_id;
    msg_id = ftok("msgfile", 65);
    int msgq_id = msgget(msg_id, 0666 | IPC_CREAT);
    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    // create readyq
    CircularQueue *readyq = malloc(sizeof(CircularQueue));
    initQueue(readyq);
    int currentquantum = 0; // reset current quantum for the first process
    // need to recieve first process to enter RR loop
    struct msgbuff receivedPCBbuff;
    printf("entering RR\n");
    int rec_val = msgrcv(msgq_id, &receivedPCBbuff, sizeof(receivedPCBbuff.pcb), 1, !IPC_NOWAIT);
    // if there is a process sent add it in the ready queue
    if (rec_val != 1)
    {
        PCB *receivedPCB = malloc(sizeof(PCB));
        memcpy(receivedPCB, &receivedPCBbuff.pcb, sizeof(PCB));
        enqueue(readyq, receivedPCB);
        printf("Received process in scheduler %d at time %d with runtime %d and priority %d \n", receivedPCB->id, getClk(), receivedPCB->runtime, receivedPCB->priority);
    }
    bool first_time = 1;
    int mq_open = 1;
    // main loop
    while (!isEmpty(readyq) || current_process != NULL || mq_open)
    {
        if (first_time)
        {
            first_time = false;
        }
        else
        {
            int currenttime = getClk();
            while (currenttime == getClk()); // kol second do the following
        }
        // usleep(200000);
        // recieve from msgq
        while (mq_open)
        {
            int rec_val = msgrcv(msgq_id, &receivedPCBbuff, sizeof(receivedPCBbuff.pcb), 0, IPC_NOWAIT);
            if (rec_val == -1)
            {
                if (errno == ENOMSG)
                {
                    // No message in the queue
                    errno = 0;
                }
                else
                {
                    mq_open = 0; // the process generator has closed the message queue
                }
                break;
            }
            // if there is a process sent add it in the ready queue
            if (rec_val != -1)
            {
                PCB *receivedPCB = malloc(sizeof(PCB));
                memcpy(receivedPCB, &receivedPCBbuff.pcb, sizeof(PCB));
                enqueue(readyq, receivedPCB);
                printf("Received process %d in scheduler  at time %d with runtime %d and priority %d \n", receivedPCB->id, getClk(), receivedPCB->runtime, receivedPCB->priority);
            }
        }

        // //lw kan fi pcb recieved, enqueue in readyq
        // if (rec_val!=-1)
        // {
        //     PCB* receivedpcb= malloc(sizeof(PCB));
        //     *receivedpcb = receivedPCBbuff.pcb;
        //     memcpy(receivedpcb, &receivedPCBbuff.pcb, sizeof(PCB));
        //     enqueue(readyq,receivedpcb);
        //     printf("received process %d at time %d with runtime %d\n",receivedpcb->id,getClk(),receivedpcb->runtime);
        // }
        // case1: fi current process that needs preemption
        if (current_process != NULL && currentquantum >= q)
        {
            kill(current_process->pid, SIGSTOP);
            current_process->state = STOPPED;
            current_process->stopped_time=getClk();
            enqueue(readyq, current_process);
            fprintf(pFile,"At time %d process %d %s arr %d total %d remain %d wait %d\n", getClk(), current_process->id, stateStrings[current_process->state], current_process->arrival_time, current_process->runtime, current_process->remaining_time,current_process->waiting_time);
            fflush(pFile);
            current_process = NULL;
        }
        if (current_process == NULL && !isEmpty(readyq)) // case2: mafish haga currently running
        {
            dequeue(readyq, &current_process);

            if (current_process->pid == -1) // never forked before->fork
            {
                current_process->state = STARTED;
                current_process->start_time=getClk();
                current_process->waiting_time=current_process->start_time-current_process->arrival_time;
                fprintf(pFile,"At time %d process %d %s arr %d total %d remain %d wait %d\n", getClk(), current_process->id, stateStrings[current_process->state], current_process->arrival_time, current_process->runtime, current_process->remaining_time,current_process->waiting_time);
                fflush(pFile);
                int currentprocesspid = fork();
                if (currentprocesspid == 0)
                {
                    char sch_id[20];
                    sprintf(sch_id, "%d", getppid());
                    char runtime[20];
                    sprintf(runtime, "%d", current_process->runtime);
                    execl("./process.out", "process.out", runtime, sch_id, NULL);
                }
                else
                {
                    current_process->pid = currentprocesspid;
                }
            }
            else // has been previously forked
            {
                current_process->state = RESUMED;
                current_process->restarted_time=getClk();
                current_process->waiting_time=current_process->restarted_time-current_process->stopped_time;
                
                fprintf(pFile,"At time %d process %d %s arr %d total %d remain %d wait %d\n", getClk(), current_process->id, stateStrings[current_process->state], current_process->arrival_time, current_process->runtime, current_process->remaining_time,current_process->waiting_time);
                fflush(pFile);
                kill(current_process->pid, SIGCONT);
            }
            currentquantum = 0; // reset quantum
        }
        if (current_process != NULL) // case3: fi current process bas msh ha preempt it
        {
            current_process->remaining_time--;
            if (current_process->remaining_time == 0)
            {
                current_process->state = FINISHED;
                current_process->finish_time=getClk();
                float TA = current_process->finish_time - current_process->arrival_time;
                float WTA = TA/current_process->runtime;
                fprintf(pFile,"At time %d process %d %s arr %d total %d remain %d wait %d TA %f WTA %f\n", getClk()+1, current_process->id, stateStrings[current_process->state], current_process->arrival_time, current_process->runtime, current_process->remaining_time,current_process->waiting_time,TA,WTA);
                fflush(pFile);
                printf("finished print %d - %d - %d\n", isEmpty(readyq), current_process->id, mq_open);
                //free(current_process);
                current_process = NULL;
                //printf("empty? %d, opened? %d\n", isEmpty(readyq), mq_open);
            }

            currentquantum++;
        }
    }
    
   
}

void MLFQ(int q)
{
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
        printf("Received process in scheduler %d at time %d with runtime %d and priority %d \n", receivedPCB->id, getClk(), receivedPCB->runtime, receivedPCB->priority);
    }
    int mq = true;
    // loop while there is still processes unfinished or the process generator didn't close the message queue
    while (!isPriEmpty(ReadyQueue) || mq || current_process != NULL)
    {
        int rec_val = msgrcv(msgq_id, &receivedPCBbuff, sizeof(receivedPCBbuff.pcb), 1, IPC_NOWAIT);
        if (errno == EIDRM)
        {
            mq = false;
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
            printf("Received process in scheduler %d at time %d with runtime %d and priority %d \n", receivedPCB->id, getClk(), receivedPCB->runtime, receivedPCB->priority);
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
                execl("./process.out", "process.out", runtime, id, NULL);
                printf("error in excel of process\n");
            }
            printf("Process: %d : \n", current_process->id);
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
    printf("Process %d finished at time %d \n", current_process->id, getClk());
    // if the process sends SIGUSR1 then the current process finished
    current_process->state = FINISHED;
    // set the current process to null
    current_process = NULL;
}
void ProcessFinishedRR(int signum)
{
    // printf("Process %d finished at time %d \n", current_process->id, getClk());
    // // if the process sends SIGUSR1 then the current process finished
    // current_process->state = FINISHED;
    // // set the current process to null
    // current_process = NULL;
}