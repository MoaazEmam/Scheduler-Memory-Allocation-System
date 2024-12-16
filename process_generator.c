#include "headers.h"

void clearResources(int);
int msgq_id;

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    if (argc < 4){
        printf("incorrect number of arguments\n");
        exit(1);
    }
    // TODO Initialization
    // 1. Read the input files.
    CircularQueue* PCBs = malloc(sizeof(CircularQueue));
    initQueue(PCBs);
    FILE *pFile;
    pFile = fopen(argv[1], "r");
    if (pFile == NULL) {
        printf("no such file.");
        return 1;
    }
    int id,arrival,runtime,priority;
    fscanf(pFile, "%*[^\n]\n");
    while (fscanf(pFile, "%d %d %d %d", &id, &arrival, &runtime, &priority) == 4) {
        PCB* readingPcb;
        readingPcb = malloc(sizeof(PCB));
        readingPcb->id = id;
        readingPcb->arrival_time = arrival;
        readingPcb->runtime = runtime;
        readingPcb->priority = priority;
        //printf("Received process %d with runtime %d and priority %d \n",readingPcb->id,readingPcb->runtime,readingPcb->priority);
        enqueue(PCBs,readingPcb);
    }
    
    //  2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
    /*  1. Shortest Job First (SJF)
        2. Preemptive Highest Priority First (HPF)
        3. Round Robin (RR)
        4. Multiple level Feedback Loop*/

    int algo_chosen = atoi(argv[3]);
    int quantum;
    //input quantum if round robin or multiple level feedback
    if (algo_chosen >= 3)
    {
        if (argc > 5)
        {
            quantum = atoi(argv[5]);
        }
        else
        {
            printf("incorrect number of arguments \n");
            exit(1);
        }
    }
    // 3. Initiate and create the scheduler and clock processes.
    //fork scheduler process
    int scheduler_id = fork();
    if (scheduler_id == 0)
    {
        // compile scheduler code
        int scheduler_compile = system("gcc scheduler.c -o scheduler.out");
        if (scheduler_compile == 0)
        {
            // the forked process now runs the scheduler
            execl("./scheduler.out", "scheduler.out",argv[3],argv[5],NULL);
            printf("failed to execl");
        }
        else
        {
            printf("Couldn't compile scheduler.c \n");
            exit(1);
        }
    }
    // fork clk process
    int clk_id = fork();
    if (clk_id == 0)
    {
        //compile clk code
        int clk_compile = system("gcc clk.c -o clk.out");
        if (clk_compile == 0)
        {
            //the forked process now runs the clk
            execl("./clk.out", "clk.out", NULL);
            printf("Fails to execl");
        }
        else
        {
            printf("Couldn't compile clk.out \n");
            exit(1);
        }
    }
    // 4. Use this function after creating the clock process to initialize clock.
    initClk();
    // To get time use this function.
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // set message queue between process generator and schedular
    key_t msg_id;
    int send_val;
    msg_id = ftok("msgfile", 65);
    msgq_id = msgget(msg_id, 0666 | IPC_CREAT);
    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    struct msgbuff arrivedprocess;
    while (!isEmpty(PCBs))
    { // loop until processes are gone
        PCB* currentPcb;
        dequeue(PCBs, &currentPcb);
        arrivedprocess.pcb.id = currentPcb->id;
        arrivedprocess.pcb.arrival_time = currentPcb->arrival_time;
        arrivedprocess.pcb.runtime = currentPcb->runtime;
        arrivedprocess.pcb.priority = currentPcb->priority;
        arrivedprocess.pcb.pid = -1;
        arrivedprocess.pcb.remaining_time = currentPcb->runtime;
        arrivedprocess.pcb.waiting_time = 0;
        arrivedprocess.pcb.state = READY; //momken ne3melha enum....3mlnaha f3lan b enum :)
        while (getClk() < currentPcb->arrival_time); //wait till a process arrives
        printf("Sent process %d with arrival time %d and runtime %d and priority %d \n",arrivedprocess.pcb.id,arrivedprocess.pcb.arrival_time,arrivedprocess.pcb.runtime,arrivedprocess.pcb.priority);
        arrivedprocess.mtype = 1;
        send_val = msgsnd(msgq_id, &arrivedprocess, sizeof(arrivedprocess.pcb), !IPC_NOWAIT); //send process to schedular
        if (send_val == -1){
            perror("Failed to send PCB to schedular \n");
        }
        free(currentPcb);
    }
    msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0); //destroy message queue after sending all processes
    int stat;
    wait(&stat);
    // 7. Clear clock resources
    destroyClk(true);
    
    return 0;
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
     // 7. Clear clock resources
    destroyClk(true);
    msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0); //destroy message queue
    raise(SIGKILL);

}
