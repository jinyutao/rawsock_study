#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "constant.h"
#include "proc_agv.h"
#include "tc8_main.h"

struct globalArgs_t globalArgs =
{
    /*.is_daemon = */0,
    /*.port = */DEFAULT_PORT,
    /*.net_if_name = */"\0",
    /*.debug = */0
};

static int stat_for_daemon();

int main(int agvc,char* agvr[])
{
    if(proc_arg(agvc, agvr) == 0)
    {
        return EXIT_SUCCESS;
    }

    if(globalArgs.is_daemon)
    {
        return stat_for_daemon();
    }
    return tc8_main_process();
}

static int stat_for_daemon()
{
    printf("Start daemon\n");
    pid_t fpid; //fpid表示fork函数返回的值
    fpid = fork();
    if (fpid < 0)
    {
        printf("error in fork!");
        return EXIT_FAILURE;
    }
    else if (fpid == 0) {
        printf("Child process is start/n");
        return tc8_main_process();
    }
    else {
        printf("Parent process will exit../n");
        return EXIT_SUCCESS;
    }
}
