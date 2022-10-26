#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>

#include "proc_agv.h"

extern struct globalArgs_t globalArgs;

static const char *optString = "t:h";
static struct option long_options[] =
{
    {"testcase", required_argument, NULL, 't'},
    {"help", no_argument, NULL, 'h'},
    {NULL, no_argument, NULL, 0}
};

stParTastName gTastName[] =
{
    {"NO18", "TCP_BASICS_13", TCP_BASICS_13,true},
    {"NO19", "TCP_BASICS_14", TCP_BASICS_14,true},
    {"NO55", "TCP_CALL_ABORT_03_03", TCP_CALL_ABORT_03_03,true},
    {NULL, NULL, MAX_TYPE},
};

stParTastName gTastDumy = {"test", "test dumy", TEST_DUMY,true};

static void print_useage()
{
    printf("usage:\n");
    printf("tc8Test [-t | --testcase]  [-h | --help]\n");
    printf("\n  -t | --testcase <case name>\n"
           "  case name:  \n");
    for(stParTastName* p = gTastName; p->testCase != MAX_TYPE; p++)
    {
        printf("    %s | %s  \n",p->NO_ID,p->TestName);
    }
    printf("\n  -h | --help \n"
           "    Show this message.\n"
           "\n");
}

int proc_arg(int argc, char* argv[])
{
    int opt;
    int option_index = 0;
    globalArgs.pTastCaseInfo = &gTastDumy;
    while ( (opt = getopt_long(argc, argv, optString, long_options, &option_index)) != EOF)
    {
        switch(opt)
        {
            case 't':
            {
                for(stParTastName* p = gTastName; p->testCase != MAX_TYPE; p++)
                {
                    if(!strcmp(optarg, p->NO_ID) ||
                        !strcmp(optarg, p->TestName))
                    {
                        globalArgs.pTastCaseInfo = p;
                        break;
                    }
                }
                break;
            }
            case 'h':
            default:
                print_useage();
                return 0;
        }
    }
    return 1;
}
