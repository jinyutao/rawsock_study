#include <string.h>           // strcpy, memset(), and memcpy()
#include <unistd.h>           // close()

#include <netdb.h>            // struct addrinfo
#include <sys/types.h>        // needed for socket(), uint8_t, uint16_t, uint32_t
#include <sys/socket.h>       // needed for socket()
#include <arpa/inet.h>        // inet_pton() and inet_ntop()
#include <sys/ioctl.h>        // macro ioctl is defined
#include <bits/ioctls.h>      // defines values for argument "request" of ioctl.
#include <net/ethernet.h>

#include "raw_sock_util.h"

uint16_t chksum(uint16_t* buff,int len)
{
    int checksum = 0;
    for(int i=0;i<len/2;i++)
    {
        checksum+=buff[i];
    }
    checksum=(checksum>>16)+(checksum & 0xffff);
    checksum+=(checksum>>16);
    return htons((uint16_t)(~checksum & 0xffff));
}

int init_row_env(raw_sock_local_env_conf* gRawSockEnvConf)
{
    struct ifreq ifr;
    int sd;

    // Submit request for a socket descriptor to look up interface.
    if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) { //第一次创建socket是为了获取本地网卡信息
        perror ("socket() failed to get socket descriptor for using ioctl() ");
        exit (EXIT_FAILURE);
    }

    // Use ioctl() to look up interface name and get its MAC address.
    memset (&ifr, 0, sizeof (ifr));
    snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", gRawSockEnvConf->local_dev);
    if (ioctl (sd, SIOCGIFHWADDR, &ifr) < 0) {
        perror ("ioctl() failed to get source MAC address ");
        exit (EXIT_FAILURE);
    }
    close (sd);

    // Copy source MAC address.
    memcpy (gRawSockEnvConf->host_mac, ifr.ifr_hwaddr.sa_data, IFHWADDRLEN);

    // Report source MAC address to stdout.
    printf ("MAC address for interface %s is ", gRawSockEnvConf->local_dev);
    for (int i=0; i<5; i++) {
        printf ("%02x:", gRawSockEnvConf->host_mac[i]);
    }
    printf ("%02x\n", gRawSockEnvConf->host_mac[5]);

    // Find interface index from interface name and store index in
    // struct sockaddr_ll device, which will be used as an argument of sendto().
    memset (&(gRawSockEnvConf->device), 0, sizeof (gRawSockEnvConf->device));
    if ((gRawSockEnvConf->device.sll_ifindex = if_nametoindex (gRawSockEnvConf->local_dev)) == 0) {
        perror ("if_nametoindex() failed to obtain interface index ");
        exit (EXIT_FAILURE);
    }
    printf ("Index for interface %s is %i\n", gRawSockEnvConf->local_dev, gRawSockEnvConf->device.sll_ifindex);

    // Submit request for a socket descriptor to look up interface.
    if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) { //第一次创建socket是为了获取本地网卡信息
        perror ("socket() failed to get socket descriptor for using ioctl() ");
        exit (EXIT_FAILURE);
    }
    return sd;
}