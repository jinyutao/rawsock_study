#include "row_session_mana.h"
#include <linux/if_ether.h>   // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD
#include <netinet/ip.h>       // struct ip and IP_MAXPACKET (which is 65535)
#include <netinet/tcp.h>       //
#include <string.h>
#include <map>

std::map<uint64_t, raw_sock_session_info*> gSessionSet;
static uint64_t getKey(uint8_t* ip, uint16_t port)
{
    union
    {
        uint64_t ret;
        uint8_t tmp[sizeof(uint64_t)];
    };
    memcpy(tmp, ip,4);
    memcpy(tmp + 4, &port, 2);
    return ret;
}

raw_sock_session_info* addSession(struct ethhdr * ethh, struct ip *iph, struct tcphdr *tcph, raw_sock_local_env_conf* pLocalEnv)
{
    uint64_t key = getKey((uint8_t*)&(iph->ip_src.s_addr), tcph->th_sport);
    if(gSessionSet.find(key) == gSessionSet.end())
    {
        raw_sock_session_info* ret = new raw_sock_session_info();
        ret->recv_ack_no=0;
        ret->recv_sn_no=0;
        ret->will_send_ack_no=0;
        ret->will_send_sn_no=0;
        ret->base_sn=0;
        memcpy(ret->RemoteMac, ethh->h_source, ETH_ALEN);
        memcpy(ret->RemoteIp, &(iph->ip_src.s_addr), 4);
        ret->RemotePort=ntohs(tcph->th_sport);
        ret->pLocalEvn=pLocalEnv;
        gSessionSet[key] = ret;
        return ret;
    }
    return NULL;
}
raw_sock_session_info* getSession(struct ip *iph, struct tcphdr *tcph, raw_sock_local_env_conf* pLocalEnv)
{
    uint64_t key = getKey((uint8_t*)&(iph->ip_src.s_addr), tcph->th_sport);
    auto it = gSessionSet.find(key);
    if(it == gSessionSet.end())
    {
        return NULL;
    }
    return it->second;
}