#ifndef RAW_SOCK_SEND_H
#define RAW_SOCK_SEND_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <netinet/in.h>       // IPPROTO_ICMP, INET_ADDRSTRLEN
#include <netinet/ip.h>       // struct ip and IP_MAXPACKET (which is 65535)
#include <netinet/tcp.h>       //
#include <netinet/udp.h>       //
#include <netinet/ip_icmp.h>  // struct icmp, ICMP_ECHO
#include <net/if.h>           // struct ifreq
#include "raw_sock_util.h"


int mk_buf_send_sync_recv(uint8_t * data_raw, int len,
    uint32_t snNo, uint32_t ackNo,
    const raw_sock_session_info* gRawSessionInfo);

int mk_buf_send_fin(uint8_t * data_raw, int len,
    uint32_t snNo, uint32_t ackNo,
    const raw_sock_session_info* gRawSessionInfo);

int mk_buf_send_ack(uint8_t * data_raw, int len,
    uint32_t snNo, uint32_t ackNo,
    const raw_sock_session_info* gRawSessionInfo);

int mk_buf_send_data(uint8_t * data_raw, int len,
    uint32_t snNo, uint32_t ackNo,
    uint8_t * sendPlayload, int PlayloadLen,
    const raw_sock_session_info* gRawSessionInfo);

int mk_buf_echoreply(uint8_t * data_raw, int len,
    uint16_t id, uint16_t sequence,
    uint8_t * pData, int datalen,
    const raw_sock_session_info* gRawSessionInfo);

int send_row_data(int sd, uint8_t * p, int len,
    const raw_sock_local_env_conf* localEnvConf);
#endif