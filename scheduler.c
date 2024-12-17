#include "headers.h"
// #include "priority_queue.h"
void RR(int q);
void MLFQ(int q);
void SJF();
void HPF();
void ProcessFinishedSJF(int signum);

PCB *current_process = NULL;
FILE *pFile;
int WTA_sum = 0;

int main(int argc, char *argv[])
{
    initClk();
    // printf("algo: %s, quan: %s \n", argv[1], argv[2]);
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

void MLFQ(int q)
{
}

void SJF()
{
    // attach handler of finished processes
    signal(SIGUSR1, ProcessFinishedSJF);
    //open output.log file
    pFile = fopen("scheduler.log", "w");
    fprintf(pFile,"#At time x process y state arr w total z remain y wait k \n");
    float waiting_sum = 0; //sum of waiting time
    float noProcess = 0; //number of processes
    float runtime_sum = 0; //sum of runtime
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
    //printf("entering sjf\n");
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
    int mq_open = 1;
    // loop while there is still processes unfinished or the process generator didn't close the message queue
    while (!isPriEmpty(ReadyQueue) || mq_open || current_process != NULL)
    {
        while (mq_open)
        {
            int rec_val = msgrcv(msgq_id, &receivedPCBbuff, sizeof(receivedPCBbuff.pcb), 1, IPC_NOWAIT);
            if (rec_val == -1)
            {
                if (errno == ENOMSG)
                {
                    // No message in the queue
                    errno = 0; // Reset errno to avoid stale values
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
                enqueuePri(ReadyQueue, receivedPCB, receivedPCB->runtime);
                printf("Received process in scheduler %d at time %d with runtime %d and priority %d \n", receivedPCB->id, getClk(), receivedPCB->runtime, receivedPCB->priority);
            }
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
            current_process->waiting_time = getClk()- current_process->arrival_time;
            waiting_sum += current_process->waiting_time;
            runtime_sum += current_process->runtime;
            noProcess++;
            current_process->start_time = getClk();
            fprintf(pFile,"At time %d process %d started arr %d total %d remain %d wait %d \n",getClk(),current_process->id,current_process->arrival_time,current_process->runtime,current_process->remaining_time,current_process->waiting_time);
            current_process->pid = current_processID;
        }
    }
    fclose(pFile);
    float cpu_utilization = (runtime_sum / (getClk()-1)) * 100;
    float avgWTA = WTA_sum / noProcess; 
    float avgWaiting = waiting_sum / noProcess;
    FILE *perf;
    perf = fopen("scheduler.perf","w");
    fprintf(perf, "CPU utilization = %.2f %% \n",cpu_utilization);
    fprintf(perf, "Avg WTA = %.2f %% \n",avgWTA);
    fprintf(perf,"Avg Waiting = %.2f %% \n",avgWaiting);
    fclose(perf);
}

void HPF()
{
}

void ProcessFinishedSJF(int signum)
{
    printf("Process %d finished at time %d \n", current_process->id, getClk());
    // if the process sends SIGUSR1 then the current process finished
    current_process->state = FINISHED;
    current_process->remaining_time = 0;
    current_process->finished_time = getClk();
    float TA = current_process->finished_time - current_process->arrival_time;
    float WTA = TA/current_process->runtime;
    WTA_sum += WTA; 
    fprintf(pFile,"At time %d process %d finished arr %d total %d remain %d wait %d TA %.2f WTA %.2f\n",getClk(),current_process->id,current_process->arrival_time,current_process->runtime,current_process->remaining_time,current_process->waiting_time,TA,WTA);
    free(current_process);
    // set the current process to null
    current_process = NULL;
}