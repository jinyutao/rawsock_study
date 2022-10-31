#ifndef PROC_ARG_H
#define PROC_ARG_H

enum enumTestCase
{
    TCP_BASICS_13,
    TCP_BASICS_14,
    TCP_CALL_ABORT_03_03,
    TCP_CALL_RECEIVE_04_01,
    TCP_CALL_RECEIVE_04_02,
    TCP_CALL_RECEIVE_04_03,
    TEST_DUMY,
    MAX_TYPE
};


struct stParTastName
{
    const char * NO_ID;
    const char * TestName;
    enumTestCase testCase;
    bool isSvr;
};

struct globalArgs_t
{
    bool debug;
    stParTastName* pTastCaseInfo;
};
extern struct globalArgs_t globalArgs;
int proc_arg(int argc, char* argv[]);
#endif //PROC_ARG_H
