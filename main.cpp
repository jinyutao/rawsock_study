#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "constant.h"
#include "proc_agv.h"
#include "tc8TestCase.h"

struct globalArgs_t globalArgs =
{
    0,
    NULL
};

int main(int agvc,char* agvr[])
{
    if(proc_arg(agvc, agvr) == 0)
    {
        return EXIT_SUCCESS;
    }
    return tc8_main_process();
}

