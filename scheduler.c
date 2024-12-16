#include "headers.h"
int msgq_id;
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
    printf("algo: %s, quan: %s \n",argv[1],argv[2]);
    // set up message queue between process generator and scheduler
    key_t msg_id;
    int send_val;
    msg_id = ftok("msgfile", 65);
    msgq_id = msgget(msg_id, 0666 | IPC_CREAT);
    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    //while(no more processes and ready queue is empty) still dk how we will implement it
    switch (toai(argv[1])){
        case 1: 
        SJF();
        break;
        
        case 2: 
        HPF();
        break;

        case 3: 
        RR(toai(argv[2]));
        break;

        case 4: 
        MLFQ(toai(argv[2]));
        break;
    }
    //the up coming loop is just for testing
    //ill leave the test loop lw 7ad 3yz yeshof setting up el msgq h comment it out bas
    // struct msgbuff sentPCB;
    // while(1){
    //     int rec_val = msgrcv(msgq_id, &sentPCB, sizeof(sentPCB.pcb), 1, !IPC_NOWAIT);
    //     if (rec_val != 1){
    //         printf("Received process in scheduler %d at time %d with runtime %d and priority %d \n",sentPCB.pcb.id,getClk(),sentPCB.pcb.runtime,sentPCB.pcb.priority);
    //     }
    // }
    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.
    
    destroyClk(true);
}

void RR(int q){

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

void SJF(){

}

void HPF(){

}