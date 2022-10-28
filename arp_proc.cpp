#include "arp_proc.h"
#include <string.h>
#include <raw_sock_send.h>
#include <linux/if_ether.h>   // ETH_P_ARP
#include "tc8TestCase.h"


int mk_buf_arpreply(uint8_t * data_raw, int len,
    struct arphdr* arph, uint8_t * ref_buff,
    const raw_sock_local_env_conf* pLocalEnvConf)
{
    // Destination and Source MAC addresses
    memcpy (data_raw, ref_buff, 6);
    memcpy (data_raw + 6, pLocalEnvConf->host_mac, 6);
    data_raw[12] = (ETH_P_ARP >> 8) & 0xFF;  // ETH_P_ARP / 256;
    data_raw[13] = ETH_P_ARP & 0xFF;         // ETH_P_ARP % 256;

    uint8_t * data = data_raw + 14;
    struct arphdr* p = (struct arphdr*)data;
    p->ar_hrd = arph->ar_hrd;
    p->ar_pro = arph->ar_pro;
    p->ar_hln = arph->ar_hln;
    p->ar_pln = arph->ar_pln;
    p->ar_op = ntohs(ARPOP_REPLY);

    data += sizeof(struct arphdr);
    memcpy (data, pLocalEnvConf->host_mac, 6);
    data += 6;
    memcpy (data, pLocalEnvConf->host_ip, 4);
    data += 4;
    memcpy (data, ref_buff, 10);
    data += 10;

    return SEND_LEN(data - data_raw);
}


void recv_arp_cb(struct ethhdr * ethh, struct arphdr* arph, uint8_t* pData, int datalen)
{
    if(arph->ar_pro != 8)
        return;
    switch (ntohs(arph->ar_op))
    {
    case ARPOP_REQUEST:
        if(!memcmp(gRawLocalEnvConf.host_ip, pData + 16, 4))
        {
            wLog( "%s %d\n", __FUNCTION__, __LINE__);
            uint8_t data[IP_MAXPACKET];
            int ret = mk_buf_arpreply(data, IP_MAXPACKET,arph,pData,&gRawLocalEnvConf);
            send_row_data(
                gRawSockRecvInfo.sFd,
                data,
                ret,
                &gRawLocalEnvConf);
        }
        break;
    case ARPOP_REPLY:
        if(!memcmp(pData + 6, pData + 16, 4))
        {
            wLog("gratuitous arp MAC:%02X:%02X:%02X:%02X:%02X:%02X IP:%d.%d.%d.%d\n",
                pData[0],pData[1],pData[2],pData[3], pData[4],pData[5],
                pData[6],pData[7],pData[8],pData[9]);
        }
        break;
    default:
        wLog("arph->ar_op : %d\n", ntohs(arph->ar_op));
        break;
    }
}
