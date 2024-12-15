#include "headers.h"

int main(int argc, char *argv[])
{
    initClk();
    printf("algo: %s, quan: %s \n",argv[1],argv[2]);
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
    //the up coming loop is just for testing
    struct msgbuff sentPCB;
    while(1){
        int rec_val = msgrcv(msgq_id, &sentPCB, sizeof(sentPCB.pcb), 1, !IPC_NOWAIT);
        if (rec_val != 1){
            printf("Received process in scheduler %d at time %d with runtime %d and priority %d \n",sentPCB.pcb.id,getClk(),sentPCB.pcb.runtime,sentPCB.pcb.priority);
        }
    }
    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.
    
    destroyClk(true);
}
