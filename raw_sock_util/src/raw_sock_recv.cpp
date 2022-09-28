
#include "raw_sock_recv.h"
#include <string.h>           // strcpy, memset(), and memcpy()

#include <netdb.h>            // struct addrinfo
#include <sys/types.h>        // needed for socket(), uint8_t, uint16_t, uint32_t
#include <sys/socket.h>       // needed for socket()
#include <arpa/inet.h>        // inet_pton() and inet_ntop()
#include <sys/ioctl.h>        // macro ioctl is defined
#include <bits/ioctls.h>      // defines values for argument "request" of ioctl.
#include <linux/if_ether.h>   // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD
#include <linux/if_packet.h>  // struct sockaddr_ll (see man 7 packet)
#include <net/ethernet.h>
#include <pthread.h>

static const raw_sock_recv_info* gCBFun = NULL;
static const raw_sock_env_conf* gEnvConf = NULL;

static void* run_recv_cb(void* p);
int create_recv_raw(
    const raw_sock_env_conf* pRawSockEnvConf,
    const raw_sock_recv_info* pRawSockRecvInfo)
{
    if(gCBFun || gEnvConf)
        return EXIT_FAILURE;

    gEnvConf = pRawSockEnvConf;
    gCBFun = pRawSockRecvInfo;
    pthread_t threadid = 0;
    pthread_attr_t attr;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&threadid, &attr, run_recv_cb,NULL);
    return EXIT_SUCCESS;
}

static int read_raw(int fd, uint8_t* pdata, int len,uint8_t** pdata_IP)
{
    // printf ("%s() len:%d\n", __FUNCTION__,len);
    int ret;
    uint16_t protocolType;
    struct sockaddr_ll device;
    socklen_t socklen = sizeof (device);
    device.sll_family = AF_PACKET;
    memcpy (device.sll_addr, gEnvConf->host_mac, IFHWADDRLEN);
    device.sll_halen = htons (IFHWADDRLEN);
    // while((ret = read(fd, pdata, len))>0)
    while((ret = recvfrom(fd, pdata, len, 0, (struct sockaddr *)&device, &socklen))>0)
    {
        if(ret >= IFHWADDRLEN + IFHWADDRLEN + 2)
        {
            memcpy(&protocolType, pdata + 6 + 6, 2);
            if(!memcmp(pdata+6, gEnvConf->remote_mac, IFHWADDRLEN) &&
                (!memcmp(pdata, gEnvConf->host_mac, IFHWADDRLEN)  ||
                    protocolType == ETH_P_ARP))
            {
                * pdata_IP = pdata + 14;
                break;
            }
            else
            {
                // char tmp[100];
                // int l = 0;
                // for(int i =0;i < MIN(ret,14);i++)
                // {
                //     l+=snprintf(tmp + l,100-l,"%02x ", pdata[i]);
                // }
                // printf ("recv() ret = %d %s\n", ret, tmp);
            }
        }
    }
    // printf ("%s() ret = %d\n", __FUNCTION__, ret);
    return ret;
}

static void* run_recv_cb(void* p)
{
    uint8_t buff[IP_MAXPACKET];
    memset(buff, 0, IP_MAXPACKET);
    uint16_t protocolType;
    while (true)
    {
        uint8_t* pdataIP;
        if(read_raw(gCBFun->sFd, buff, IP_MAXPACKET, &pdataIP) <= 0)
        {
            continue;
        }
        // check IP
        memcpy(&protocolType, buff + 6 + 6, 2);
        protocolType = ntohs(protocolType);
        switch (protocolType)
        {
        case ETH_P_IP:
        {
            if((pdataIP[0] & 0xF0) != 0x40)
            {
                continue;
            }
            struct ip *iph = (struct ip *)pdataIP;
            switch(iph->ip_p)
            {
            case IPPROTO_TCP:
            {
                // check TCP
                uint8_t*  pdataTCP = pdataIP + (pdataIP[0] & 0x0F) * 4 ;
                struct tcphdr *tcph = (struct tcphdr *)pdataTCP;
                int offset = iph->ip_hl *4 + tcph->doff*4;
                if(gCBFun->recvTcpFun)
                    gCBFun->recvTcpFun(iph, tcph,
                        pdataTCP + offset, ntohs(iph->ip_len) - offset);
                break;
            }
            case IPPROTO_UDP:
            {
                uint8_t*  pdataUDP = pdataIP + (pdataIP[0] & 0x0F) * 4 ;
                struct udphdr *udph = (struct udphdr *)pdataUDP;
                if(gCBFun->recvUdpFun)
                    gCBFun->recvUdpFun(iph, udph,
                        pdataUDP + sizeof(udphdr), ntohs(udph->len) - sizeof(udphdr));
                break;
            }
            case IPPROTO_ICMP:
            {
                uint8_t*  picmpICMP = pdataIP + (pdataIP[0] & 0x0F) * 4 ;
                struct icmphdr *icmph = (struct icmphdr *)picmpICMP;
                if(gCBFun->recvIcmpFun)
                    gCBFun->recvIcmpFun(iph, icmph,
                        picmpICMP + sizeof(icmphdr), ntohs(iph->ip_len) - (pdataIP[0] & 0x0F) * 4 - sizeof(icmphdr));
                break;
            }
            }
            break;
        }
        case ETH_P_ARP:
        {
            break;
        }
        default:
        {
            printf("protocolType : %d %02x\n", protocolType, protocolType);
            break;
        }
        }
    }
    return NULL;
}
