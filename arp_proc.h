#ifndef ARP_PROC_H
#define ARP_PROC_H

#include <raw_sock_recv.h>

extern raw_sock_env_conf gRawSockEnvConf;
extern raw_sock_recv_info gRawSockRecvInfo;
void recv_arp_cb(struct arphdr* arph, uint8_t* pData, int datalen);

#endif //ARP_PROC_H