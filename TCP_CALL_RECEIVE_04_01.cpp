#include <string.h>
#include <unistd.h>
#include <linux/if_ether.h>   // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD
#include <mutex>
#include <condition_variable>
#include "tc8TestCase.h"
#include "proc_agv.h"
#include "raw_sock_recv.h"
#include "raw_sock_send.h"

static bool canSend_CALL_RECEIVE_04_01 = false;

static std::mutex              gLockTCP_CALL_RECEIVE_04_01;
static std::condition_variable gNotifyTCP_CALL_RECEIVE_04_01;

int tc8_TCP_CALL_RECEIVE_04_01()
{
    int datalen;
    printf("wait fist push\n");
    uint8_t payload[]={'s','s','z'};

    {
        std::unique_lock<std::mutex> lk(gLockTCP_CALL_RECEIVE_04_01);
        while(!canSend_CALL_RECEIVE_04_01)
            gNotifyTCP_CALL_RECEIVE_04_01.wait(lk);

        usleep(50*1000);
    }
    // getchar();

    printf("Connected send data 3 * 5\n");

    datalen = mk_buf_send_data(data, IP_MAXPACKET,
        gpTestRemoteSessionInfo->will_send_sn_no,
        gpTestRemoteSessionInfo->will_send_ack_no,
        payload,3,
        gpTestRemoteSessionInfo);
    send_row_data(gRawSockRecvInfo.sFd, data, datalen, &gRawLocalEnvConf);
    gpTestRemoteSessionInfo->will_send_sn_no += 3;

    datalen = mk_buf_send_data(data, IP_MAXPACKET,
        gpTestRemoteSessionInfo->will_send_sn_no,
        gpTestRemoteSessionInfo->will_send_ack_no,
        payload,3,
        gpTestRemoteSessionInfo);
    send_row_data(gRawSockRecvInfo.sFd, data, datalen, &gRawLocalEnvConf);
    gpTestRemoteSessionInfo->will_send_sn_no += 3;

    datalen = mk_buf_send_data(data, IP_MAXPACKET,
        gpTestRemoteSessionInfo->will_send_sn_no,
        gpTestRemoteSessionInfo->will_send_ack_no,
        payload,3,
        gpTestRemoteSessionInfo);
    send_row_data(gRawSockRecvInfo.sFd, data, datalen, &gRawLocalEnvConf);
    gpTestRemoteSessionInfo->will_send_sn_no += 3;

    datalen = mk_buf_send_data(data, IP_MAXPACKET,
        gpTestRemoteSessionInfo->will_send_sn_no,
        gpTestRemoteSessionInfo->will_send_ack_no,
        payload,3,
        gpTestRemoteSessionInfo);
    send_row_data(gRawSockRecvInfo.sFd, data, datalen, &gRawLocalEnvConf);
    gpTestRemoteSessionInfo->will_send_sn_no += 3;

    datalen = mk_buf_send_data(data, IP_MAXPACKET,
        gpTestRemoteSessionInfo->will_send_sn_no,
        gpTestRemoteSessionInfo->will_send_ack_no,
        payload,3,
        gpTestRemoteSessionInfo);
    send_row_data(gRawSockRecvInfo.sFd, data, datalen, &gRawLocalEnvConf);
    gpTestRemoteSessionInfo->will_send_sn_no += 3;


    printf("send fin\n");
    // getchar();
    while(gpTestRemoteSessionInfo->will_send_sn_no > gpTestRemoteSessionInfo->recv_ack_no)
    {
        usleep(1000);
    }

    datalen = mk_buf_send_fin(data, IP_MAXPACKET,
        gpTestRemoteSessionInfo->will_send_sn_no,
        gpTestRemoteSessionInfo->will_send_ack_no,
        gpTestRemoteSessionInfo);
    send_row_data(gRawSockRecvInfo.sFd, data, datalen, &gRawLocalEnvConf);
    gpTestRemoteSessionInfo->will_send_sn_no += 1;
    printf("BYEBYE\n");
    getchar();
    return 0;
}
void on_cansend_TCP_CALL_RECEIVE_04_01()
{
    printf("on_cansend_TCP_CALL_RECEIVE_04_01() \n");
    std::unique_lock<std::mutex> lk(gLockTCP_CALL_RECEIVE_04_01);
    if(!canSend_CALL_RECEIVE_04_01)
    {
        canSend_CALL_RECEIVE_04_01 = true;
        gNotifyTCP_CALL_RECEIVE_04_01.notify_all();
    }
}