#include "icmp_proc.h"
#include <string.h>
#include <raw_sock_send.h>


void recv_icmp_cb(struct ip *iph, struct icmphdr *icmph, uint8_t* pData, int datalen)
{
    printf("recv_icmp_cb From %d.%d.%d.%d to %d.%d.%d.%d\n",
        gRawSockEnvConf.remote_ip[0],gRawSockEnvConf.remote_ip[1],gRawSockEnvConf.remote_ip[2],gRawSockEnvConf.remote_ip[3],
        gRawSockEnvConf.host_ip[0],gRawSockEnvConf.host_ip[1],gRawSockEnvConf.host_ip[2],gRawSockEnvConf.host_ip[3]);
    printf("             type:%d code:%d datalen:%d\n",
        icmph->type,icmph->code, datalen);
    switch(icmph->type)
    {
    case ICMP_ECHO:
        printf("             id:%d sequence:%d \n",
        ntohs(icmph->un.echo.id),ntohs(icmph->un.echo.sequence));
        uint8_t data[IP_MAXPACKET];
        int ret = mk_buf_echoreply(data, IP_MAXPACKET,
            icmph->un.echo.id,icmph->un.echo.sequence, pData, datalen, &gRawSockEnvConf);
        send_row_data(
            gRawSockRecvInfo.sFd,
            data,
            ret,
            &gRawSockEnvConf);
        break;
    }
}


