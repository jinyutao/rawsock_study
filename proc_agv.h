#ifndef PROC_ARG_H
#define PROC_ARG_H


struct globalArgs_t
{
    int is_daemon;
    int port;
    char net_if_name[128];
    int debug;
};
extern struct globalArgs_t globalArgs;
int proc_arg(int argc, char* argv[]);
#endif //PROC_ARG_H
