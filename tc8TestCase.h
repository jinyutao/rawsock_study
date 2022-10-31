#ifndef TC8_TEST_CASE_H
#define TC8_TEST_CASE_H

#include "proc_agv.h"
#include "raw_sock_recv.h"
#include "raw_sock_send.h"
#include "arp_proc.h"
#include "icmp_proc.h"

extern raw_sock_local_env_conf gRawLocalEnvConf;
extern raw_sock_recv_info gRawSockRecvInfo;

extern raw_sock_session_info* gpTestRemoteSessionInfo;
extern uint8_t data[IP_MAXPACKET];

int tc8_main_process();

int tc8_TCP_BASICS_13();

int tc8_TCP_BASICS_14();
bool on_can_recvACK_TCP_BASICS_14();

int tc8_TCP_CALL_ABORT_03_03();

int tc8_TCP_CALL_RECEIVE_04_01();
void on_cansend_TCP_CALL_RECEIVE_04_01();

int tc8_TCP_CALL_RECEIVE_04_02();
void on_cansend_TCP_CALL_RECEIVE_04_02();

int tc8_TCP_CALL_RECEIVE_04_03();
void on_cansend_TCP_CALL_RECEIVE_04_03();

int tc8_unknow();

#define wLog(...) \
do \
{   \
    if(globalArgs.debug) \
    { \
        printf(__VA_ARGS__); \
    } \
} while(0)

#endif // TC8_TEST_CASE_H