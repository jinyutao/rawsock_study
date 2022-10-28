#include <string.h>
#include <linux/if_ether.h>   // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD
#include <mutex>
#include <condition_variable>
#include "tc8TestCase.h"
#include "proc_agv.h"
#include "raw_sock_recv.h"
#include "raw_sock_send.h"

static bool canSend_CALL_RECEIVE_04_03 = false;
static int finCnt = 0;

static std::mutex              gLockTCP_CALL_RECEIVE_04_03;
static std::condition_variable gNotifyTCP_CALL_RECEIVE_04_03;

int tc8_TCP_CALL_RECEIVE_04_03()
{
    int datalen;
    printf("wait fist push\n");
    uint8_t payload[]={'s','s','z'};

    {
        std::unique_lock<std::mutex> lk(gLockTCP_CALL_RECEIVE_04_03);
        while(!canSend_CALL_RECEIVE_04_03)
            gNotifyTCP_CALL_RECEIVE_04_03.wait(lk);
    }
    // getchar();

    send_row_data(
        gRawSockRecvInfo.sFd,
        data,
        mk_buf_send_ack(data, IP_MAXPACKET,
            gpTestRemoteSessionInfo->will_send_sn_no,
            gpTestRemoteSessionInfo->will_send_ack_no,
            gpTestRemoteSessionInfo),
        &gRawLocalEnvConf);

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

    printf("BYEBYE\n");
    getchar();
    datalen = mk_buf_send_fin(data, IP_MAXPACKET,
        gpTestRemoteSessionInfo->will_send_sn_no,
        gpTestRemoteSessionInfo->will_send_ack_no,
        gpTestRemoteSessionInfo);
    send_row_data(gRawSockRecvInfo.sFd, data, datalen, &gRawLocalEnvConf);
    getchar();
    return 0;
}
void on_cansend_TCP_CALL_RECEIVE_04_03()
{
    std::unique_lock<std::mutex> lk(gLockTCP_CALL_RECEIVE_04_03);
    finCnt ++;
    if(!canSend_CALL_RECEIVE_04_03 && finCnt>=2)
    {
        canSend_CALL_RECEIVE_04_03 = true;
        gNotifyTCP_CALL_RECEIVE_04_03.notify_all();
    }
}