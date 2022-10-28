#include <string.h>
#include <linux/if_ether.h>   // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD
#include <mutex>
#include <condition_variable>
#include "tc8TestCase.h"
#include "row_session_mana.h"
#include "proc_agv.h"

raw_sock_local_env_conf gRawLocalEnvConf;
raw_sock_session_info* gpTestRemoteSessionInfo;

raw_sock_remote_info testRemoteInfo;

uint8_t data[IP_MAXPACKET];
static void recv_tcp_cb(struct ethhdr * ethh, struct ip *iph, struct tcphdr *tcph, uint8_t* pData, int datalen);
static void sync_recv(struct ethhdr * ethh, struct ip *iph, struct tcphdr *tcph);
static void data_recv(struct ip *iph, struct tcphdr *tcph, uint8_t* pData, int datalen, raw_sock_session_info*);

std::mutex              gLock;
std::condition_variable gNotify;
raw_sock_recv_info gRawSockRecvInfo =
{
    0,
    recv_tcp_cb,
    NULL,
    recv_icmp_cb,
    recv_arp_cb
};
void initEnvConf()
{
    strcpy(gRawLocalEnvConf.local_dev, "enp0s25");

    gRawLocalEnvConf.host_ip[0] = 0xc0;
    gRawLocalEnvConf.host_ip[1] = 0xa8;
    gRawLocalEnvConf.host_ip[2] = 0x00;
    gRawLocalEnvConf.host_ip[3] = 0x09; //  0x65;

    gRawLocalEnvConf.host_port = 17018;
    gRawSockRecvInfo.sFd =  init_row_env(&gRawLocalEnvConf);

    testRemoteInfo.remote_mac[0] = 0x02;//设置目的网卡地址
    testRemoteInfo.remote_mac[1] = 0x00;
    testRemoteInfo.remote_mac[2] = 0x00;
    testRemoteInfo.remote_mac[3] = 0x0b;
    testRemoteInfo.remote_mac[4] = 0x00;
    testRemoteInfo.remote_mac[5] = 0x03;

    testRemoteInfo.remote_ip[0] = 0xc0;
    testRemoteInfo.remote_ip[1] = 0xa8;
    testRemoteInfo.remote_ip[2] = 0x00;
    testRemoteInfo.remote_ip[3] = 0x04;
}

int tc8_main_process()
{
    initEnvConf();

    create_recv_raw(&gRawLocalEnvConf, &gRawSockRecvInfo);
    if(!globalArgs.pTastCaseInfo)
    {
        return tc8_unknow();
    }
    printf("Test Case : %s\nstart Test. \n", globalArgs.pTastCaseInfo->TestName);
    switch(globalArgs.pTastCaseInfo->testCase)
    {
        case TCP_BASICS_13:
            return tc8_TCP_BASICS_13();
        case TCP_BASICS_14:
            return tc8_TCP_BASICS_14();
        case TCP_CALL_ABORT_03_03:
            return tc8_TCP_CALL_ABORT_03_03();
        case TCP_CALL_RECEIVE_04_02:
            return tc8_TCP_CALL_RECEIVE_04_02();
        default: break;
    }
    return tc8_unknow();
}

int tc8_unknow()
{
    int datalen;
    printf("wait fist push\n");
    // getchar();
    // {
    //     std::unique_lock<std::mutex> lk(gLock);
    //     gNotify.wait(lk);
    //     datalen = mk_buf_send_ack(data, IP_MAXPACKET,
    //         will_send_sn_no, will_send_ack_no, &gRawLocalEnvConf);
    //     send_row_data(gRawSockRecvInfo.sFd, data, datalen, &gRawLocalEnvConf);
    //     printf("sent fist ACK\n");
    // }

    getchar();

    datalen = mk_buf_send_fin(data, IP_MAXPACKET,
        gpTestRemoteSessionInfo->will_send_sn_no, gpTestRemoteSessionInfo->will_send_ack_no, gpTestRemoteSessionInfo);
    send_row_data(gRawSockRecvInfo.sFd, data, datalen, &gRawLocalEnvConf);
    gpTestRemoteSessionInfo->will_send_sn_no ++;
    getchar();

    // datalen = mk_buf_send_fin(data, IP_MAXPACKET,
    //     will_send_sn_no, will_send_ack_no, &gRawLocalEnvConf);

    datalen = mk_buf_send_ack(data, IP_MAXPACKET,
                gpTestRemoteSessionInfo->will_send_sn_no, gpTestRemoteSessionInfo->will_send_ack_no, gpTestRemoteSessionInfo);
    send_row_data(gRawSockRecvInfo.sFd, data, datalen, &gRawLocalEnvConf);
    gpTestRemoteSessionInfo->will_send_sn_no ++;

    printf("BYEBYE\n");
    getchar();
    return 0;
}
static void recv_tcp_cb(struct ethhdr * ethh, struct ip *iph, struct tcphdr *tcph, uint8_t* pData, int datalen)
{
    uint8_t* remote_ip = (uint8_t*)&(iph->ip_src.s_addr);
    uint8_t* host_ip = (uint8_t*)&(iph->ip_dst.s_addr);
    if(memcmp(host_ip, gRawLocalEnvConf.host_ip, 4))
    {
        return;
    }
    printf("sync_recv From %d.%d.%d.%d:%d to %d.%d.%d.%d:%d\n",
        remote_ip[0],remote_ip[1],remote_ip[2],remote_ip[3], ntohs(tcph->th_sport),
        host_ip[0],host_ip[1],host_ip[2],host_ip[3], ntohs(tcph->th_dport));


    printf("seq: %u ack_seq:%u \n flags: ",
        ntohl(tcph->th_seq),
        ntohl(tcph->th_ack));

    if (tcph->th_flags & TH_SYN)
        printf("SYN ");
    if (tcph->th_flags & TH_ACK)
        printf("ACK ");
    if (tcph->th_flags & TH_FIN)
        printf("FIN ");
    if (tcph->th_flags & TH_RST)
        printf("RST ");
    if (tcph->th_flags & TH_URG)
        printf("URG ");
    if (tcph->th_flags & TH_PUSH)
        printf("PUSH ");
    printf("\n");
    printf("data len :%d \n",datalen);

    raw_sock_session_info* pSessionInfo = getSession(iph, tcph, &gRawLocalEnvConf);

    if (tcph->th_flags & TH_SYN)
    {
        if(!pSessionInfo)
        {
            sync_recv(ethh, iph, tcph);
        }
        else if(tcph->th_flags & TH_ACK)
        {
            pSessionInfo->recv_ack_no = ntohl(tcph->th_ack);
            pSessionInfo->recv_sn_no = ntohl(tcph->th_seq);
            pSessionInfo->will_send_ack_no = pSessionInfo->recv_sn_no + 1; // need test
            // pSessionInfo->will_send_sn_no = pSessionInfo->ack_no;
            send_row_data(
                gRawSockRecvInfo.sFd,
                data,
                mk_buf_send_ack(data, IP_MAXPACKET,
                    pSessionInfo->will_send_sn_no, pSessionInfo->will_send_ack_no, pSessionInfo),
                &gRawLocalEnvConf);
        }
        // else send RST?
        return;
    }
    if((tcph->th_flags == TH_ACK) && pSessionInfo)
    {
        bool flg = (pSessionInfo->will_send_sn_no == pSessionInfo->base_sn+1);
        pSessionInfo->recv_ack_no = ntohl(tcph->th_ack);
        pSessionInfo->recv_sn_no = ntohl(tcph->th_seq);
        pSessionInfo->will_send_ack_no = pSessionInfo->recv_sn_no;
        // pSessionInfo->will_send_sn_no = pSessionInfo->ack_no;
        if(gpTestRemoteSessionInfo == pSessionInfo)
        {
            switch (globalArgs.pTastCaseInfo->testCase)
            {
            case TCP_BASICS_14:
                if(flg)
                {
                    datalen = mk_buf_send_fin(data, IP_MAXPACKET,
                        pSessionInfo->will_send_sn_no, pSessionInfo->will_send_ack_no, pSessionInfo);
                    send_row_data(gRawSockRecvInfo.sFd, data, datalen, &gRawLocalEnvConf);
                    pSessionInfo->will_send_sn_no ++;
                }
                break;
            default:
                break;
            }
        }
    }
    if(tcph->th_flags & TH_PUSH)
    {
        data_recv(iph, tcph, pData, datalen, pSessionInfo);
    }
    if(tcph->th_flags & TH_FIN)
    {
        if(gpTestRemoteSessionInfo == pSessionInfo)
        {
            switch (globalArgs.pTastCaseInfo->testCase)
            {
            case TCP_BASICS_14:
                if(!on_can_recvACK_TCP_BASICS_14())
                {
                    return;
                }
                break;
            case TCP_CALL_RECEIVE_04_02:
                on_cansend_TCP_CALL_RECEIVE_04_02();
                return;
            default:
                break;
            }
        }

        pSessionInfo->recv_ack_no = ntohl(tcph->th_ack);
        pSessionInfo->recv_sn_no = ntohl(tcph->th_seq);

        pSessionInfo->will_send_ack_no = pSessionInfo->recv_sn_no + 1;
        // pSessionInfo->will_send_sn_no = pSessionInfo->ack_no;

        send_row_data(
            gRawSockRecvInfo.sFd,
            data,
            mk_buf_send_ack(data, IP_MAXPACKET,
                pSessionInfo->will_send_sn_no,
                pSessionInfo->will_send_ack_no,
                pSessionInfo),
            &gRawLocalEnvConf);


    }
}

static void sync_recv(struct ethhdr * ethh, struct ip *iph, struct tcphdr *tcph)
{
    raw_sock_session_info* pSessionInfo = addSession(ethh, iph, tcph, &gRawLocalEnvConf);
    if(!gpTestRemoteSessionInfo)
    {
        if(!memcmp(ethh->h_source, testRemoteInfo.remote_mac, IFHWADDRLEN) &&
            !memcmp(&(iph->ip_src.s_addr), testRemoteInfo.remote_ip, 4))
        {
            gpTestRemoteSessionInfo = pSessionInfo;
        }
    }
    pSessionInfo->recv_ack_no = ntohl(tcph->th_ack);
    pSessionInfo->recv_sn_no = ntohl(tcph->th_seq);

    pSessionInfo->will_send_ack_no = pSessionInfo->recv_sn_no + 1;
    pSessionInfo->will_send_sn_no = pSessionInfo->recv_sn_no + 1024;
    if(pSessionInfo->base_sn == 0)
    {
        pSessionInfo->base_sn = pSessionInfo->will_send_sn_no;
    }

    int datalen = mk_buf_send_sync_recv(data, IP_MAXPACKET,
        pSessionInfo->will_send_sn_no, pSessionInfo->will_send_ack_no, pSessionInfo);
    send_row_data(gRawSockRecvInfo.sFd, data, datalen, &gRawLocalEnvConf);
    pSessionInfo->will_send_sn_no++;
}

static void data_recv(struct ip *iph, struct tcphdr *tcph, uint8_t* pData, int datalen, raw_sock_session_info* pSessionInfo)
{
    pSessionInfo->recv_ack_no = ntohl(tcph->th_ack);
    pSessionInfo->recv_sn_no = ntohl(tcph->th_seq);

    pSessionInfo->will_send_ack_no = pSessionInfo->recv_sn_no + datalen;
    // pSessionInfo->will_send_sn_no = pSessionInfo->ack_no;

    {
        std::unique_lock<std::mutex> lk(gLock);
        gNotify.notify_all();
    }
}
