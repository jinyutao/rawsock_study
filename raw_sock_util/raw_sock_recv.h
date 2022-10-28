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
#include <linux/if_ether.h>   // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD
#include <net/if_arp.h>
#include "raw_sock_util.h"

typedef void (*recv_tcp)(struct ethhdr * ethh, struct ip *iph, struct tcphdr *tcph, uint8_t* pData, int datalen);
typedef void (*recv_udp)(struct ethhdr * ethh, struct ip *iph, struct udphdr *udph, uint8_t* pData, int datalen);
typedef void (*recv_icmp)(struct ethhdr * ethh, struct ip *iph, struct icmphdr *icmph, uint8_t* pData, int datalen);
typedef void (*recv_arp)(struct ethhdr * ethh, struct arphdr* arph, uint8_t* pData, int datalen);
struct raw_sock_recv_info
{
    int     sFd;
    recv_tcp recvTcpFun;
    recv_udp recvUdpFun;
    recv_icmp recvIcmpFun;
    recv_arp recvArpFun;
};

int create_recv_raw(
    const raw_sock_local_env_conf* pRawSockEnvConf,
    const raw_sock_recv_info* pRawSockRecvInfo);

#endif // RAW_SOCK_RECV_H