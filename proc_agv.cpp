#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>

#include "proc_agv.h"

extern struct globalArgs_t globalArgs;

static const char *optString = "Dp:n:h";
static struct option long_options[] =
{
    {"daemon",  no_argument, NULL, 'D'},
    {"port",  required_argument, NULL, 'p'},
    {"net-if", required_argument, NULL, 'n'},
    {"debug",  no_argument, NULL, 'd'},
    {"help", no_argument, NULL, 'h'},
    {NULL, no_argument, NULL, 0}
};

static void print_useage()
{
    printf("usage:\n");
    printf("ntpd [-D | --daemon] [-p | --port <port number>] [-n | --net-if <net interface name>] [-h | --help]\n");
    printf("\n  -D | --daemon \n"
           "\n  -p | --port <port number> \n"
           "\n  -n | --net-if <net interface name> \n"
           "\n  -h | --help \n"
           "    Show this message.\n"
           "\n");
}

int proc_arg(int argc, char* argv[])
{
    int opt;
    int option_index = 0;
    while ( (opt = getopt_long(argc, argv, optString, long_options, &option_index)) != EOF)
    {
        switch(opt)
        {
            case 'D':
            {
                globalArgs.is_daemon = 1;
                break;
            }
            case 'p':
            {
                globalArgs.port = atoi(optarg);
                if(!(0 < globalArgs.port && globalArgs.port < 65535))
                {
                    print_useage();
                    return 0;
                }
                break;
            }
            case 'n':
                strncpy(globalArgs.net_if_name, optarg,128-1);
                break;
            case 'd':
                globalArgs.debug = 1;
                break;
            case 'h':
            default:
                print_useage();
                return 0;
        }
    }
    return 1;
}
