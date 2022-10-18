#ifndef ICMP_PROC_H
#define ICMP_PROC_H

#include <raw_sock_recv.h>

extern raw_sock_env_conf gRawSockEnvConf;
extern raw_sock_recv_info gRawSockRecvInfo;
void recv_icmp_cb(struct ip *iph, struct icmphdr *icmph, uint8_t* pData, int datalen);

#endif //ICMP_PROC_H