#ifndef RAW_SOCK_RECV_H
#define RAW_SOCK_RECV_H

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
typedef void (*recv_tcp)(struct ip *iph, struct tcphdr *tcph, uint8_t* pData, int datalen);
typedef void (*recv_udp)(struct ip *iph, struct udphdr *tcph, uint8_t* pData, int datalen);
typedef void (*recv_icmp)(struct ip *iph,  uint8_t* pData, int datalen);
struct raw_sock_recv_info
{
    int     sFd;
    recv_tcp recvTcpFun;
    recv_udp recvUdpFun;
};

int create_recv_raw(
    const raw_sock_env_conf* pRawSockEnvConf,
    const raw_sock_recv_info* pRawSockRecvInfo);

#endif // RAW_SOCK_RECV_H