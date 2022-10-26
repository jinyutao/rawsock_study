#ifndef TC8_TEST_CASE_H
#define TC8_TEST_CASE_H

#include "raw_sock_recv.h"
#include "raw_sock_send.h"
#include "arp_proc.h"
#include "icmp_proc.h"

extern uint32_t ack_no;
extern uint32_t sn_no;
extern uint32_t will_send_ack_no;
extern uint32_t will_send_sn_no;
extern raw_sock_env_conf gRawSockEnvConf;
extern uint8_t data[IP_MAXPACKET];

int tc8_main_process();
int tc8_TCP_BASICS_13();
int tc8_TCP_BASICS_14();
int tc8_TCP_CALL_ABORT_03_03();
bool on_can_recvACK_TCP_BASICS_14();
int tc8_unknow();
#endif // TC8_TEST_CASE_H