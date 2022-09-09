#ifndef RAW_SOCK_UTIL_H
#define RAW_SOCK_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <net/if.h>           // struct ifreq
#include <linux/if_packet.h>  // struct sockaddr_ll (see man 7 packet)

struct raw_sock_env_conf
{
    char local_dev[IFNAMSIZ];
    uint8_t host_mac[IFHWADDRLEN];
    uint8_t remote_mac[IFHWADDRLEN];
    uint8_t host_ip[4];
    uint8_t remote_ip[4];
    uint16_t host_port;
    uint16_t remote_port;
    struct sockaddr_ll device;
};

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

int init_row_env(raw_sock_env_conf* gRawSockEnvConf);
uint16_t chksum(uint16_t* buff,int len);

#endif // RAW_SOCK_UTIL_H