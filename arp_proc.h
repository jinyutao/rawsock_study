#ifndef ARP_PROC_H
#define ARP_PROC_H

#include <raw_sock_recv.h>

extern raw_sock_local_env_conf gRawLocalEnvConf;
extern raw_sock_recv_info gRawSockRecvInfo;
void recv_arp_cb(struct ethhdr * ethh, struct arphdr* arph, uint8_t* pData, int datalen);

#endif //ARP_PROC_H