#include "headers.h"
#include "circular_queue.h"
void clearResources(int);
int msgq_id;

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    CircularQueue *PCBs;
    initQueue(PCBs);
    // FILE *pFile;
    // pFile = fopen("processes.txt", "r");
    //  2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
    /*  1. Shortest Job First (SJF)
        2. Preemptive Highest Priority First (HPF)
        3. Round Robin (RR)
        4. Multiple level Feedback Loop*/
    int algo_chosen = atoi(argv[1]);
    int quantum;
    // input quantum if round robin or multiple level feedback
    if (algo_chosen >= 3)
    {
        if (argc > 2)
        {
            quantum = atoi(argv[2]);
        }
        else
        {
            printf("incorrect number of arguments \n");
            exit(1);
        }
    }
    else
    {
        quantum = 0;
    }
    // 3. Initiate and create the scheduler and clock processes.
    // fork scheduler process
    int scheduler_id = fork();
    if (scheduler_id == 0)
    {
        // compile scheduler code
        int scheduler_compile = system("gcc scheduler.c -o scheduler.out");
        if (scheduler_compile == 0)
        {
            // the forked process now runs the scheduler
            execl("./scheduler.out", "scheduler.out", algo_chosen, quantum, NULL);
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
        // compile clk code
        int clk_compile = system("gcc clk.c -o clk.out");
        if (clk_compile == 0)
        {
            // the forked process now runs the clk
            execl("./clk.out", "clk.out", NULL);
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
        PCB currentPcb;
        dequeue(PCBs, && currentPcb);
        while (getClk()<currentPcb.arrival_time); //wait till a process arrives
        arrivedprocess.mtype = 0;
        arrivedprocess.pcb = currentPcb;
        send_val = msgsnd(msgq_id, &arrivedprocess, sizeof(arrivedprocess.pcb), !IPC_NOWAIT); //send process to schedular
        if (send_val == -1){
            perror("Failed to send PCB to schedular \n");
        }
    }
    // 7. Clear clock resources
    destroyClk(true);
    msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0); //destroy message queue
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
     // 7. Clear clock resources
    destroyClk(true);
    msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0); //destroy message queue
    raise(SIGKILL);

}
