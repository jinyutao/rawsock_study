#include "icmp_proc.h"
#include <string.h>
#include <raw_sock_send.h>


void recv_icmp_cb(struct ethhdr * ethh, struct ip *iph, struct icmphdr *icmph, uint8_t* pData, int datalen)
{
    uint8_t* remote_ip = (uint8_t*)&(iph->ip_src.s_addr);
    uint8_t* host_ip = (uint8_t*)&(iph->ip_dst.s_addr);

    printf("recv_icmp_cb From %d.%d.%d.%d to %d.%d.%d.%d\n",
        remote_ip[0],remote_ip[1],remote_ip[2],remote_ip[3],
        host_ip[0],host_ip[1],host_ip[2],host_ip[3]);
    printf("             type:%d code:%d datalen:%d\n",
        icmph->type,icmph->code, datalen);
    switch(icmph->type)
    {
    case ICMP_ECHO:
        printf("             id:%d sequence:%d \n",
        ntohs(icmph->un.echo.id),ntohs(icmph->un.echo.sequence));
        uint8_t data[IP_MAXPACKET];
        raw_sock_session_info rawSessionInfo;
        memcpy(&rawSessionInfo.remoteInfo.remote_mac, ethh->h_source, ETH_ALEN);
        memcpy(&rawSessionInfo.remoteInfo.remote_ip, &(iph->ip_src.s_addr), 4);
        rawSessionInfo.pLocalEvn = &gRawLocalEnvConf;
        int ret = mk_buf_echoreply(data, IP_MAXPACKET,
            icmph->un.echo.id,icmph->un.echo.sequence, pData, datalen, &rawSessionInfo);
        send_row_data(
            gRawSockRecvInfo.sFd,
            data,
            ret,
            &gRawLocalEnvConf);
        break;
    }
}


