#include "headers.h"
// #include "priority_queue.h"
void RR(int q);
void MLFQ(int q);
void SJF();
void HPF();
void ProcessFinishedSJF(int signum);
void ProcessFinishedRR(int signum);
void generatorTerminate(int signum);
BuddyMemory *memory;
PCB *current_process = NULL;
FILE *pFile;
FILE *mFile;
float WTA_sum = 0;
int mq_open = 1;

bool arrIsEmpty(CircularQueue *arr[], int size)
{
    for (int i = 0; i < size; i++)
    {
        if (arr[i]->rear != NULL) // If any queue is not empty
            return false;
    }
    return true; // All queues are empty
}

int main(int argc, char *argv[])
{
    initClk();
    // attach handler of finished process generator
    signal(SIGUSR2, generatorTerminate);
    memory = malloc(sizeof(BuddyMemory));
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

    // TODO: implement the scheduler.
    // TODO: upon termination release the clock resources.

    destroyClk(true);
    return 0;
}

void RR(int q)
{
    signal(SIGUSR1, ProcessFinishedRR);
    // open log file
    pFile = fopen("scheduler.log", "w");
    fprintf(pFile, "#At time x process y state arr w total z remain y wait k \n");
    fflush(pFile);
    float waiting_sum = 0; // sum of waiting time
    float noProcess = 0;   // number of processes
    float runtime_sum = 0; // sum of runtime
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
    //bool first_time = 1;
     //int mq_open = 1;
    //  main loop
    while (!isEmpty(readyq) || current_process != NULL || mq_open)
    {
         int currenttime = getClk();
       
        // usleep(200000);
        // if (first_time)
        // {
        //     first_time = false;
        // }
        // else
        // {
        //     int currenttime = getClk();
            
        // }
       
        
        while (1)
        {
            int rec_val = msgrcv(msgq_id, &receivedPCBbuff, sizeof(receivedPCBbuff.pcb), 0, IPC_NOWAIT);
             if (rec_val != -1)
            {
                PCB *receivedPCB = malloc(sizeof(PCB));
                memcpy(receivedPCB, &receivedPCBbuff.pcb, sizeof(PCB));
                enqueue(readyq, receivedPCB);
                printf("Received process %d in scheduler  at time %d with runtime %d and priority %d \n", receivedPCB->id, getClk(), receivedPCB->runtime, receivedPCB->priority);
            }
            else if (rec_val==-1)
             {
                if (errno == ENOMSG)
                {
                    // No message in the queue
                    errno = 0;
                }
                // else
                // {
                //     msgq_open = 0; // the process generator has closed the message queue
                // }
                break;
             }
            
           
        }

       
        // case1: fi current process that needs preemption
        if (current_process != NULL && currentquantum >= q)
        {
            kill(current_process->pid, SIGSTOP);
            current_process->state = STOPPED;
            current_process->stopped_time = getClk();
            enqueue(readyq, current_process);
            fprintf(pFile, "At time %d process %d %s arr %d total %d remain %d wait %d\n", getClk(), current_process->id, stateStrings[current_process->state], current_process->arrival_time, current_process->runtime, current_process->remaining_time, current_process->waiting_time);
            fflush(pFile);
            current_process = NULL;
        }
        if (current_process == NULL && !isEmpty(readyq)) // case2: mafish haga currently running
        {
            dequeue(readyq, &current_process);

            if (current_process->pid == -1) // never forked before->fork
            {
                current_process->state = STARTED;
                current_process->start_time = getClk();
                noProcess++;
                current_process->waiting_time = getClk() - current_process->arrival_time;
                runtime_sum += current_process->runtime;
                waiting_sum += current_process->waiting_time;
                fprintf(pFile, "At time %d process %d %s arr %d total %d remain %d wait %d\n", getClk(), current_process->id, stateStrings[current_process->state], current_process->arrival_time, current_process->runtime, current_process->remaining_time, current_process->waiting_time);
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
                current_process->restarted_time = getClk();
                waiting_sum -= current_process->waiting_time;
                current_process->waiting_time += current_process->restarted_time - current_process->stopped_time;
                waiting_sum += current_process->waiting_time;

                fprintf(pFile, "At time %d process %d %s arr %d total %d remain %d wait %d\n", getClk(), current_process->id, stateStrings[current_process->state], current_process->arrival_time, current_process->runtime, current_process->remaining_time, current_process->waiting_time);
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
                current_process->finished_time = getClk();
                float TA = current_process->finished_time - current_process->arrival_time;
                float WTA = TA / current_process->runtime;
                WTA_sum += WTA;
                fprintf(pFile, "At time %d process %d %s arr %d total %d remain %d wait %d TA %.2f WTA %.2f\n", getClk() + 1, current_process->id, stateStrings[current_process->state], current_process->arrival_time, current_process->runtime, current_process->remaining_time, current_process->waiting_time, TA, WTA);
                fflush(pFile);
                printf("finished print %d - %d - %d\n", isEmpty(readyq), current_process->id, mq_open);
                // free(current_process);
                current_process = NULL;
                // printf("empty? %d, opened? %d\n", isEmpty(readyq), mq_open);
            }

            currentquantum++;
        }
         while (currenttime == getClk())// kol second do the following
         {
            usleep(500);
         }
         
    }
    fclose(pFile);
    float cpu_utilization = (runtime_sum / (getClk() - 1)) * 100;
    float avgWTA = WTA_sum / noProcess;
    float avgWaiting = waiting_sum / noProcess;
    FILE *perf;
    perf = fopen("scheduler.perf", "w");
    fprintf(perf, "CPU utilization = %.2f %% \n", cpu_utilization);
    fprintf(perf, "Avg WTA = %.2f \n", avgWTA);
    fprintf(perf, "Avg Waiting = %.2f \n", avgWaiting);
    fclose(perf);
}

void MLFQ(int q)
{
    signal(SIGUSR1, ProcessFinishedSJF);
    bool readyqEmpty = false;
    PCB *waiting_process = NULL;
    CircularQueue *queuearray[11]; // array holding queues for each priority level
    CircularQueue *waitingqueue = malloc(sizeof(CircularQueue));
    initQueue(waitingqueue);
    memory->memsize = 1024;
    memory->start=0;
    int current_level = 0;
    pFile = fopen("scheduler.log", "w");
    mFile = fopen("memory.log", "w");
    fprintf(mFile, "#At time x allocated y bytes for process z from i to j \n");
    fflush(mFile);
    fprintf(pFile, "#At time x process y state arr w total z remain y wait k \n");
    fflush(pFile);
    float waiting_sum = 0; // sum of waiting time
    float noProcess = 0;   // number of processes
    float runtime_sum = 0; // sum of runtime
    printf("MLFQ is running....\n");
    // for loop to create 11 queues (pri levels 0 to 10)
    for (int i = 0; i < 11; i++)
    {
        CircularQueue *q = malloc(sizeof(CircularQueue));
        initQueue(q);
        queuearray[i] = q;
    }
    struct msgbuff receivedPCBbuff;
    ; // struct to recieve pcb from pgen

    // setting up msg queue with process gen
    key_t msg_id;
    int send_val;
    msg_id = ftok("msgfile", 65);
    int msgq_id = msgget(msg_id, 0666 | IPC_CREAT);
    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    // wait for first process to be sent
    printf("Waiting for first process to be sent\n");
    int rec_val = msgrcv(msgq_id, &receivedPCBbuff, sizeof(receivedPCBbuff.pcb), 1, !IPC_NOWAIT);
    printf("rec_val = %d\n", rec_val);
    //  if there is a process sent add it in the ready queue
    if (rec_val != 1)
    {
        noProcess++;
        PCB *receivedPCB = malloc(sizeof(PCB));
        memcpy(receivedPCB, &receivedPCBbuff.pcb, sizeof(PCB));
        // printf("now enqueing process %d at %d\n",receivedPCB->id,getClk());
        allocate(memory, receivedPCB, receivedPCB->memsize, memory->start);
        enqueue(queuearray[receivedPCB->priority], receivedPCB);
        printf("Received process in scheduler %d at time %d with runtime %d and priority %d \n", receivedPCB->id, getClk(), receivedPCB->runtime, receivedPCB->priority);
    }
    // int msgq_open = 1;
    PCB *new_process = malloc(sizeof(PCB));
    int currentprocessID;
    while (!isEmpty(waitingqueue) || !arrIsEmpty(queuearray, 11) || mq_open == 1 || current_process != NULL)
    {
        // printf("In big while\n");
        while (mq_open)
        {
            // printf("in small while\n");
            rec_val = msgrcv(msgq_id, &receivedPCBbuff, sizeof(receivedPCBbuff.pcb), 1, IPC_NOWAIT);
            // printf("%d %d %d\n",getClk(),rec_val,errno);
            // printf("rec_val = %d\n",rec_val);
            if (rec_val == -1)
            {
                if (errno == ENOMSG)
                {
                    // No message in the queue
                    errno = 0;
                }
                // else
                // {
                //     msgq_open = 0; // the process generator has closed the message queue
                // }
                break;
            }
            else //(rec_val != -1)
            {
                noProcess++;
                PCB *receivedPCB = malloc(sizeof(PCB));
                memcpy(receivedPCB, &receivedPCBbuff.pcb, sizeof(PCB));
                // printf("now enqueing process %d at %d\n",receivedPCB->id,getClk());
                // enqueue(queuearray[receivedPCB->priority], receivedPCB);
                enqueue(waitingqueue, receivedPCB);
                printf("Received process in scheduler %d at time %d with runtime %d and priority %d \n", receivedPCB->id, getClk(), receivedPCB->runtime, receivedPCB->priority);
            }
        }
        if (!isEmpty(waitingqueue))
        {
            peak(waitingqueue, &waiting_process);
            if (allocate(memory, waiting_process, waiting_process->memsize, memory->start))
            {
                dequeue(waitingqueue, &waiting_process);
                enqueue(queuearray[waiting_process->priority], waiting_process);
            }
        }
        if (current_process == NULL && !arrIsEmpty(queuearray, 11))
        {
            // find process to start
            current_level = 0;
            while (1)
            {
                if (queuearray[current_level]->rear != NULL)
                    break;
                current_level++;
            }
            if (current_level < 11)
            {
                // printf("dequeuing process %d at %d\n",queuearray[current_level]->rear->pcb->id,getClk());
                dequeue(queuearray[current_level], &current_process);
                // printf("dequeued proces %d at %d\n",current_process->id,getClk());
                if (current_process != NULL)
                {

                    if (current_process->state == STOPPED)
                    {
                        // totalruntime = current_process->runtime - current_process->remaining_time;
                        // printf("writing in file\n");
                        //  printf("Restarting process %d\n", current_process->id);
                        current_process->state = RUNNING;
                        current_process->restarted_time = getClk();
                        currentprocessID = current_process->pid;
                        waiting_sum -= current_process->waiting_time;
                        current_process->waiting_time += getClk() - current_process->stopped_time;
                        waiting_sum += current_process->waiting_time;
                        fprintf(pFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk(), current_process->id, current_process->arrival_time, current_process->runtime, current_process->remaining_time, current_process->waiting_time);
                        fflush(pFile);
                        kill(currentprocessID, SIGCONT);
                    }
                    else if (current_process->state == READY)
                    {
                        runtime_sum += current_process->runtime;
                        current_process->waiting_time = getClk() - current_process->arrival_time;
                        waiting_sum += current_process->waiting_time;
                        current_process->state = RUNNING;
                        current_process->start_time = getClk();
                        current_process->restarted_time = getClk();
                        // totalruntime=0;
                        current_process->remaining_time = current_process->runtime;
                        fprintf(mFile, "At time %d allocated %d bytes for process %d from %d to %d \n", getClk(), current_process->memsize, current_process->id, current_process->start_address, current_process->end_address);
                        fflush(mFile);
                        fprintf(pFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), current_process->id, current_process->arrival_time, current_process->runtime, current_process->remaining_time, current_process->waiting_time);
                        fflush(pFile);
                        currentprocessID = fork();
                        if (currentprocessID == 0)
                        {
                            // run it and send the process the scheduler's id
                            char id[20];
                            sprintf(id, "%d", getppid());
                            char runtime[20];
                            sprintf(runtime, "%d", current_process->runtime);
                            execl("./process.out", "process.out", runtime, id, NULL);
                            printf("error in excel of process\n");
                        }
                        printf("Process with id %d running\n", current_process->id);
                        current_process->pid = currentprocessID;
                    }
                }
            }
        }
        else if (current_process != NULL && getClk() - current_process->restarted_time >= q)
        {
            if (current_process->state == RUNNING)
            {
                // printf("quantum passed on running process %d.....stopping\n", current_process->id);
                current_process->state = STOPPED;
                current_process->stopped_time = getClk();
                current_process->remaining_time -= q;
                currentprocessID = current_process->pid;
                fprintf(pFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk(), current_process->id, current_process->arrival_time, current_process->runtime, current_process->remaining_time, current_process->waiting_time);
                fflush(pFile);
                kill(currentprocessID, SIGSTOP);
                if (current_level < 10)
                {
                    // printf("enqueuing process %d at %d into level %d\n",current_process->id,getClk(),current_level+1);
                    enqueue(queuearray[current_level + 1], current_process);
                    // printf("enqueued process %d at %d into level %d\n",current_process->id,getClk(),current_level+1);
                }
                else
                {
                    // printf("enqueuing process %d at %d into level %d\n",current_process->id,getClk(),current_level);
                    enqueue(queuearray[current_process->priority], current_process);
                    // printf("enqueued process %d at %d into level %d\n",current_process->id,getClk(),current_level);
                }
                current_process = NULL;
            }
        }
        // printf("array empty:%d msgq_open:%d current null? : %d\n",!arrIsEmpty(queuearray, 11),msgq_open,current_process==NULL);
        // printf("finish iter of big while");
    }
    fclose(pFile);
    fclose(mFile);
    float cpu_utilization = (runtime_sum / (getClk())) * 100;
    float avgWTA = WTA_sum / noProcess;
    float avgWaiting = waiting_sum / noProcess;
    FILE *perf;
    perf = fopen("scheduler.perf", "w");
    fprintf(perf, "CPU utilization = %.2f %% \n", cpu_utilization);
    fprintf(perf, "Avg WTA = %.2f \n", avgWTA);
    fprintf(perf, "Avg Waiting = %.2f \n", avgWaiting);
    fclose(perf);
}

void SJF()
{
    // attach handler of finished processes
    signal(SIGUSR1, ProcessFinishedSJF);
    // open output.log file
    pFile = fopen("scheduler.log", "w");
    fprintf(pFile, "#At time x process y state arr w total z remain y wait k \n");
    float waiting_sum = 0; // sum of waiting time
    float noProcess = 0;   // number of processes
    float runtime_sum = 0; // sum of runtime
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

    // create a ready priority queue
    PriorityQueue *ReadyQueue = createQueue();
    struct msgbuff receivedPCBbuff;
    // printf("entering sjf\n");
    //  wait for first process to arrive to start the algo
    int rec_val = msgrcv(msgq_id, &receivedPCBbuff, sizeof(receivedPCBbuff.pcb), 1, !IPC_NOWAIT);
    // if there is a process sent add it in the ready queue
    if (rec_val != 1)
    {
        PCB *receivedPCB = malloc(sizeof(PCB));
        memcpy(receivedPCB, &receivedPCBbuff.pcb, sizeof(PCB));
        enqueuePri(ReadyQueue, receivedPCB, receivedPCB->runtime);
        // printf("Received process in scheduler %d at time %d with runtime %d and priority %d \n", receivedPCB->id, getClk(), receivedPCB->runtime, receivedPCB->priority);
    }
    // int mq_open = 1;
    //  loop while there is still processes unfinished or the process generator didn't close the message queue
    while (!isPriEmpty(ReadyQueue) || mq_open || current_process != NULL)
    {
        // sleep(0.2);
        while (mq_open)
        {
            int rec_val = msgrcv(msgq_id, &receivedPCBbuff, sizeof(receivedPCBbuff.pcb), 1, IPC_NOWAIT);
            if (rec_val == -1)
            {
                if (errno == ENOMSG)
                {
                    // No message in the queue
                    errno = 0;
                }
                // else
                // {
                //     mq_open = 0; // the process generator has closed the message queue
                // }
                break;
            }
            // if there is a process sent add it in the ready queue
            if (rec_val != -1)
            {
                PCB *receivedPCB = malloc(sizeof(PCB));
                memcpy(receivedPCB, &receivedPCBbuff.pcb, sizeof(PCB));
                enqueuePri(ReadyQueue, receivedPCB, receivedPCB->runtime);
                // printf("Received process in scheduler %d at time %d with runtime %d and priority %d \n", receivedPCB->id, getClk(), receivedPCB->runtime, receivedPCB->priority);
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
                // printf("error in excel of process\n");
            }
            current_process->waiting_time = getClk() - current_process->arrival_time;
            waiting_sum += current_process->waiting_time;
            runtime_sum += current_process->runtime;
            noProcess++;
            current_process->start_time = getClk();
            fprintf(pFile, "At time %d process %d started arr %d total %d remain %d wait %d \n", getClk(), current_process->id, current_process->arrival_time, current_process->runtime, current_process->remaining_time, current_process->waiting_time);
            current_process->pid = current_processID;
        }
    }
    fclose(pFile);
    float cpu_utilization = (runtime_sum / (getClk())) * 100;
    float avgWTA = WTA_sum / noProcess;
    float avgWaiting = waiting_sum / noProcess;
    FILE *perf;
    perf = fopen("scheduler.perf", "w");
    fprintf(perf, "CPU utilization = %.2f %% \n", cpu_utilization);
    fprintf(perf, "Avg WTA = %.2f \n", avgWTA);
    fprintf(perf, "Avg Waiting = %.2f \n", avgWaiting);
    fclose(perf);
}

void HPF()
{
    PriorityQueue *ReadyQueue = createQueue(); // Priority queue for processes
    PCB *current_process = NULL;
    FILE *logfile = fopen("scheduler.log", "w");
    struct msgbuff receivedPCBbuff;
    int msgq_id;
    int totalWaitTime = 0, totalTurnaroundTime = 0, totalProcesses = 0, rec_val;
    float WTA_sum = 0, WTA = 0;
    float runtime_sum = 0;
    key_t msg_id = ftok("msgfile", 65);
    msgq_id = msgget(msg_id, 0666 | IPC_CREAT);
    if (msgq_id == -1)
    {
        perror("Error while creating the message queue for HPF");
        exit(-1);
    }

    // Initialize the clock and get the starting time
    // initClk();
    int clockTime = getClk();

    // Create the message queue
    // while (getClk() != 1);
    fprintf(logfile, "#At time x process y state arr w total z remain y wait k\n");

    while (!isPriEmpty(ReadyQueue) || mq_open || current_process != NULL)
    {
        // Check for new processes in the message queue
        if (isPriEmpty(ReadyQueue) && !current_process)
        {
            rec_val = msgrcv(msgq_id, &receivedPCBbuff, sizeof(receivedPCBbuff.pcb), 1, 0);
        }
        else
        {
            sleep(0.2);
            rec_val = msgrcv(msgq_id, &receivedPCBbuff, sizeof(receivedPCBbuff.pcb), 1, IPC_NOWAIT);
        }
        if (rec_val == -1)
        {
            if (errno == ENOMSG)
            {
                // No message in the queue
                errno = 0; // Reset errno to avoid stale values
            }
            // else
            // {
            //     mq_open = 0; // the process generator has closed the message queue
            // }
        }

        if (rec_val != -1)
        {
            PCB *receivedPCB = malloc(sizeof(PCB));
            memset(receivedPCB, 0, sizeof(PCB));
            memcpy(receivedPCB, &receivedPCBbuff.pcb, sizeof(PCB));
            enqueuePri(ReadyQueue, receivedPCB, receivedPCB->priority);
            receivedPCB->remaining_time = receivedPCB->runtime;
            printf("Received process with id %d at time %d with runtime %d and priority %d\n",
                   receivedPCB->id, getClk(), receivedPCB->runtime, receivedPCB->priority);
        }

        // Handle the currently running process
        if (current_process)
        {
            if ((current_process->remaining_time - 1) == 0)
            {
                // Process finished
                fprintf(logfile, "At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f\n", getClk(), current_process->id, current_process->arrival_time, current_process->runtime, current_process->waiting_time, clockTime - current_process->arrival_time, (float)(clockTime - current_process->arrival_time) / current_process->runtime);

                totalWaitTime += current_process->waiting_time;
                totalTurnaroundTime += getClk() - current_process->arrival_time;
                WTA = (float)(getClk() - current_process->arrival_time) / current_process->runtime;
                WTA_sum += WTA;
                totalProcesses++;
                current_process->state = FINISHED;
                free(current_process);
                current_process = NULL;
            }
            else
            {
                current_process->remaining_time = current_process->remainingTimeAfterStop - (getClk() - current_process->restarted_time);
            }
        }

        // ba select process lw no process fel ready aw nakhod haga b priority 3alya
        if (current_process && !isPriEmpty(ReadyQueue) && ReadyQueue->front->priority < current_process->priority)
        {
            // bahnadle el preemption
            fprintf(logfile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n",
                    getClk(), current_process->id, current_process->arrival_time, current_process->runtime,
                    current_process->remaining_time, current_process->waiting_time);
            kill(current_process->pid, SIGSTOP);
            current_process->remainingTimeAfterStop = current_process->remaining_time;
            current_process->stopped_time = getClk();

            current_process->state = STOPPED;
            enqueuePri(ReadyQueue, current_process, current_process->priority);
            current_process = NULL;
        }

        if (!current_process && !isPriEmpty(ReadyQueue))
        {
            dequeuePri(ReadyQueue, &current_process);

            if (current_process->state == READY)
            {
                // Start a new process
                current_process->remaining_time = current_process->runtime;
                current_process->remainingTimeAfterStop = current_process->runtime;
                runtime_sum += current_process->runtime;
                current_process->restarted_time = getClk();
                current_process->pid = fork();
                if (current_process->pid == 0)
                {
                    while (1)
                        pause(); // Waiting for SIGCONT
                }
                // Update wait time
                current_process->waiting_time += (getClk() - current_process->arrival_time);

                fprintf(logfile, "At time %d process %d started arr %d total %d remain %d wait %d\n",
                        getClk(), current_process->id, current_process->arrival_time, current_process->runtime,
                        current_process->remaining_time, current_process->waiting_time);
            }
            else if (current_process->state == STOPPED)
            {
                // Resume a previously stopped process
                kill(current_process->pid, SIGCONT);
                current_process->waiting_time += (getClk() - current_process->stopped_time); // Correct wait time
                current_process->restarted_time = getClk();

                fprintf(logfile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n",
                        getClk(), current_process->id, current_process->arrival_time, current_process->runtime,
                        current_process->remaining_time, current_process->waiting_time);
            }
            current_process->state = RUNNING; // harg3ha running baa whatever heia gdeeda wala preempted
        }

        // Advance the clock
        clockTime++;
        while (getClk() < clockTime)
            ;
    }

    // Log performance metrics
    float cpu_utilization = (runtime_sum / (getClk() - 1)) * 100;
    float avgWTA = WTA_sum / totalProcesses;
    float avgWaiting = (float)totalWaitTime / totalProcesses;
    FILE *perf;
    perf = fopen("scheduler.perf", "w");
    fprintf(perf, "CPU utilization = %.2f %% \n", cpu_utilization);
    fprintf(perf, "Avg WTA = %.2f \n", avgWTA);
    fprintf(perf, "Avg Waiting = %.2f \n", avgWaiting);
    fclose(perf);
    fclose(logfile);

    freePriQueue(ReadyQueue);
    destroyClk(true); // Release shared clock resources
}

void ProcessFinishedSJF(int signum)
{
    printf("Process %d finished at time %d \n", current_process->id, getClk());
    deallocate(memory, current_process->start_address);
    // if the process sends SIGUSR1 then the current process finished
    current_process->state = FINISHED;
    current_process->remaining_time = 0;
    current_process->finished_time = getClk();
    float TA = current_process->finished_time - current_process->arrival_time;
    float WTA = TA / current_process->runtime;
    WTA_sum += WTA;
    fprintf(mFile, "At time %d freed %d bytes for process %d from %d to %d \n", getClk(), current_process->memsize, current_process->id, current_process->start_address, current_process->end_address);
    fflush(mFile);
    fprintf(pFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %.2f WTA %.2f\n", getClk(), current_process->id, current_process->arrival_time, current_process->runtime, current_process->remaining_time, current_process->waiting_time, TA, WTA);
    fflush(pFile);
    free(current_process);
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

void generatorTerminate(int signum)
{
    mq_open = 0;
}