#include <string.h>
#include <linux/if_ether.h>   // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD
#include <mutex>
#include <condition_variable>
#include "tc8TestCase.h"
#include "proc_agv.h"
#include "raw_sock_recv.h"
#include "raw_sock_send.h"
bool canSendAck = false;
int tc8_TCP_BASICS_14()
{
    int datalen;
    canSendAck = false;

    printf("canSendAck ...\n");
    getchar();

    printf("set canSendAck to true\n");
    canSendAck = true;

    getchar();

    datalen = mk_buf_send_fin(data, IP_MAXPACKET,
        gpTestRemoteSessionInfo->will_send_sn_no,
        gpTestRemoteSessionInfo->will_send_ack_no,
        gpTestRemoteSessionInfo);
    send_row_data(gRawSockRecvInfo.sFd, data, datalen, &gRawLocalEnvConf);
    gpTestRemoteSessionInfo->will_send_sn_no ++;

    printf("BYEBYE\n");
    getchar();
    return 0;
}

bool on_can_recvACK_TCP_BASICS_14()
{
    printf("canSendAck? %c\n", canSendAck?'T':'F');
    return canSendAck;
}