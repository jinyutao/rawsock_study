#include <string.h>
#include <linux/if_ether.h>   // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD
#include <mutex>
#include <condition_variable>
#include "tc8TestCase.h"
#include "proc_agv.h"
#include "raw_sock_recv.h"
#include "raw_sock_send.h"

int tc8_TCP_CALL_ABORT_03_03()
{
    int datalen;
    printf("wait fist push\n");

    getchar();

    datalen = mk_buf_send_fin(data, IP_MAXPACKET,
        will_send_sn_no, will_send_ack_no, &gRawSockEnvConf);
    send_row_data(gRawSockRecvInfo.sFd, data, datalen, &gRawSockEnvConf);

    getchar();

    // datalen = mk_buf_send_fin(data, IP_MAXPACKET,
    //     will_send_sn_no, will_send_ack_no, &gRawSockEnvConf);

    datalen = mk_buf_send_ack(data, IP_MAXPACKET,
                will_send_sn_no, will_send_ack_no, &gRawSockEnvConf);
    send_row_data(gRawSockRecvInfo.sFd, data, datalen, &gRawSockEnvConf);

    printf("BYEBYE\n");
    getchar();
    return 0;
}
