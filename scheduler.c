#include "headers.h"

int main(int argc, char *argv[])
{
    initClk();
    // set up message queue between process generator and schedular
    key_t msg_id;
    int send_val;
    msg_id = ftok("msgfile", 65);
    msgq_id = msgget(msg_id, 0666 | IPC_CREAT);
    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    //TODO: implement the scheduler.
    //TODO: upon termination release the clock resources.
    
    destroyClk(true);
}
