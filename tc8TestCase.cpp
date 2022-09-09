#include <string.h>
#include <linux/if_ether.h>   // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD
#include <mutex>
#include <condition_variable>
#include "tc8TestCase.h"
#include "raw_sock_recv.h"
#include "raw_sock_send.h"


uint32_t ack_no;
uint32_t sn_no;
uint32_t will_send_ack_no;
uint32_t will_send_sn_no;
raw_sock_env_conf gRawSockEnvConf;

uint8_t data[IP_MAXPACKET];
static void recv_tcp_cb(struct ip *iph, struct tcphdr *tcph, uint8_t* pData, int datalen);
static void sync_recv(struct tcphdr *tcph);
static void data_recv(struct tcphdr *tcph, uint8_t* pData, int datalen);

std::mutex              gLock;
std::condition_variable gNotify;
raw_sock_recv_info gRawSockRecvInfo =
{
    0,
    recv_tcp_cb,
    NULL
};
void initEnvConf()
{
    strcpy(gRawSockEnvConf.local_dev, "enp0s25");
    gRawSockEnvConf.remote_mac[0] = 0x02;//设置目的网卡地址
    gRawSockEnvConf.remote_mac[1] = 0x00;
    gRawSockEnvConf.remote_mac[2] = 0x00;
    gRawSockEnvConf.remote_mac[3] = 0x0b;
    gRawSockEnvConf.remote_mac[4] = 0x00;
    gRawSockEnvConf.remote_mac[5] = 0x03;

    gRawSockEnvConf.host_ip[0] = 0xc0;
    gRawSockEnvConf.host_ip[1] = 0xa8;
    gRawSockEnvConf.host_ip[2] = 0x00;
    gRawSockEnvConf.host_ip[3] = 0x0a; //  0x65;
    gRawSockEnvConf.remote_ip[0] = 0xc0;
    gRawSockEnvConf.remote_ip[1] = 0xa8;
    gRawSockEnvConf.remote_ip[2] = 0x00;
    gRawSockEnvConf.remote_ip[3] = 0x04;

    gRawSockEnvConf.host_port = 17018;
    gRawSockRecvInfo.sFd =  init_row_env(&gRawSockEnvConf);
}

int tc8_main_process()
{
    int datalen;
    initEnvConf();

    create_recv_raw(&gRawSockEnvConf, &gRawSockRecvInfo);

    printf("wait fist push\n");
    // getchar();
    // {
    //     std::unique_lock<std::mutex> lk(gLock);
    //     gNotify.wait(lk);
    //     datalen = mk_buf_send_ack(data, IP_MAXPACKET,
    //         will_send_sn_no, will_send_ack_no, &gRawSockEnvConf);
    //     send_row_data(gRawSockRecvInfo.sFd, data, datalen, &gRawSockEnvConf);
    //     printf("sent fist ACK\n");
    // }

    getchar();

    datalen = mk_buf_send_fin(data, IP_MAXPACKET,
        will_send_sn_no, will_send_ack_no, &gRawSockEnvConf);
    send_row_data(gRawSockRecvInfo.sFd, data, datalen, &gRawSockEnvConf);

    printf("BYEBYE\n");
    getchar();
    return 0;
}

static void recv_tcp_cb(struct ip *iph, struct tcphdr *tcph, uint8_t* pData, int datalen)
{
    printf("sync_recv From %d.%d.%d.%d:%d to %d.%d.%d.%d:%d\n",
        gRawSockEnvConf.remote_ip[0],gRawSockEnvConf.remote_ip[1],gRawSockEnvConf.remote_ip[2],gRawSockEnvConf.remote_ip[3], ntohs(tcph->th_sport),
        gRawSockEnvConf.host_ip[0],gRawSockEnvConf.host_ip[1],gRawSockEnvConf.host_ip[2],gRawSockEnvConf.host_ip[3], ntohs(tcph->th_dport));

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

    if (tcph->th_flags & TH_SYN)
    {
        sync_recv(tcph);
        return;
    }
    if(tcph->th_flags & TH_PUSH)
    {
        data_recv(tcph, pData, datalen);
    }
    if(tcph->th_flags & TH_FIN)
    {
        ack_no = ntohl(tcph->th_ack);
        sn_no = ntohl(tcph->th_seq);

        will_send_ack_no = sn_no + 1;
        will_send_sn_no = ack_no;
        int datalen = mk_buf_send_ack(data, IP_MAXPACKET,
            will_send_sn_no, will_send_ack_no, &gRawSockEnvConf);

        send_row_data(gRawSockRecvInfo.sFd, data, datalen, &gRawSockEnvConf);
    }
}

static void sync_recv(struct tcphdr *tcph)
{
    if(gRawSockEnvConf.remote_port)
    {
        return;
    }
    ack_no = ntohl(tcph->th_ack);
    sn_no = ntohl(tcph->th_seq);
    gRawSockEnvConf.remote_port = ntohs(tcph->th_sport);

    will_send_ack_no = sn_no + 1;
    will_send_sn_no = sn_no + 1024;

    int datalen = mk_buf_send_sync_recv(data, IP_MAXPACKET,
        will_send_sn_no, will_send_ack_no, &gRawSockEnvConf);

    send_row_data(gRawSockRecvInfo.sFd, data, datalen, &gRawSockEnvConf);
}

static void data_recv(struct tcphdr *tcph, uint8_t* pData, int datalen)
{
    if(gRawSockEnvConf.remote_port != ntohs(tcph->th_sport))
    {
        return;
    }
    ack_no = ntohl(tcph->th_ack);
    sn_no = ntohl(tcph->th_seq);

    will_send_ack_no = sn_no + datalen;
    will_send_sn_no = ack_no;

    {
        std::unique_lock<std::mutex> lk(gLock);
        gNotify.notify_all();
    }
}
