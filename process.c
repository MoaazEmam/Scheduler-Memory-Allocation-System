#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int time;

void myHandler(int sig_num){
    time = getClk();
}

int main(int agrc, char *argv[])
{
    initClk();
    signal(SIGCONT,myHandler);
    printf("Running \n");
    //TODO The process needs to get the remaining time from somewhere
    remainingtime = atoi(argv[1]); //schedular sends the runtime as an argument
    time = getClk();
    while (remainingtime > 0)
    {
        if (getClk() != time){
            remainingtime -= getClk()-time;
            time = getClk();
        }
    }
    kill (atoi(argv[2]),SIGUSR1); //send signal to scheduler that it finished
    destroyClk(false);
    return 0;
}
