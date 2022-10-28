#ifndef RAW_SOCK_UTIL_H
#define RAW_SOCK_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <net/if.h>           // struct ifreq
#include <linux/if_packet.h>  // struct sockaddr_ll (see man 7 packet)

struct raw_sock_local_env_conf
{
    char local_dev[IFNAMSIZ];
    uint8_t host_mac[IFHWADDRLEN];
    uint8_t host_ip[4];
    uint16_t host_port;
    struct sockaddr_ll device;
};

struct raw_sock_remote_info
{
    uint8_t remote_mac[IFHWADDRLEN];
    uint8_t remote_ip[4];
    uint16_t remote_port;
};

struct raw_sock_session_info
{
    uint32_t recv_ack_no;
    uint32_t recv_sn_no;
    uint32_t will_send_ack_no;
    uint32_t will_send_sn_no;
    uint32_t base_sn;

    raw_sock_remote_info remoteInfo;
    #define RemoteMac remoteInfo.remote_mac
    #define RemoteIp remoteInfo.remote_ip
    #define RemotePort remoteInfo.remote_port
    raw_sock_local_env_conf* pLocalEvn;
    #define HostMac pLocalEvn->host_mac
    #define HostIp pLocalEvn->host_ip
    #define HostPort pLocalEvn->host_port
    #define HostDevice pLocalEvn->device
};

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#define SEND_LEN(l) ((l)<(46)?(46):(l))

int init_row_env(raw_sock_local_env_conf* gRawSockEnvConf);
uint16_t chksum(uint16_t* buff,int len);

#endif // RAW_SOCK_UTIL_H