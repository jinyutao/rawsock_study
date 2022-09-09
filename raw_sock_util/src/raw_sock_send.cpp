#include <string.h>
#include <linux/if_ether.h>   // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD

#include "raw_sock_send.h"

#define SEND_LEN(l) ((l)<(46)?(46):(l))

int cp_eth_info(uint8_t * data_raw, int len,
    const raw_sock_env_conf* gRawSockEnvConf)
{
    // Destination and Source MAC addresses
    memcpy (data_raw, gRawSockEnvConf->remote_mac, 6);
    memcpy (data_raw + 6, gRawSockEnvConf->host_mac, 6);
    data_raw[12] = (ETH_P_IP >> 8) & 0xFF;          // ETH_P_IP / 256;
    data_raw[13] = ETH_P_IP & 0xFF;   // ETH_P_IP % 256;
    return 14;
}

int mk_buf_send_sync_recv(uint8_t * data_raw, int len,
    uint32_t snNo, uint32_t ackNo,
    const raw_sock_env_conf* gRawSockEnvConf)
{
    int bufflen = 40;
    uint8_t byte2[2];

    snNo = htonl(snNo);
    ackNo = htonl(ackNo);
    uint16_t hostPort = htons(gRawSockEnvConf->host_port);
    uint16_t remotePort = htons(gRawSockEnvConf->remote_port);

    int ethlen = cp_eth_info(data_raw, len, gRawSockEnvConf);

    uint8_t * data = data_raw + ethlen;
    data[0] = 0x45;
    data[1] = 0;
    data[2] = ((uint8_t*)&bufflen)[1];
    data[3] = ((uint8_t*)&bufflen)[0];
    data[4] = 0x00;
    data[5] = 0x00;
    data[6] = 0x00;
    data[7] = 0x00;
    data[8] = 0x40;
    data[9] = 0x06;
    data[10] = 0;//chk sum;
    data[11] = 0;//chk sum;
    memcpy(data + 12, gRawSockEnvConf->host_ip, 4);
    memcpy(data + 16, gRawSockEnvConf->remote_ip, 4);

    uint16_t chk = chksum((uint16_t *)data, 20);
    data[10] = (chk >> 8) & 0xFF;
    data[11] = chk & 0xFF;

    memcpy(byte2, &hostPort, 2);
    data[20] = byte2[0]; // 17018
    data[21] = byte2[1]; // 17018
    memcpy(byte2, &remotePort, 2);
    data[22] = byte2[0]; // dport
    data[23] = byte2[1]; // dport
    // data[24] = 0x00; // snm
    // data[25] = 0xea; // snm
    // data[26] = 0x0d; // snm
    // data[27] = 0x66; // snm
    memcpy(data + 24, &snNo, 4);
    // data[28] = 0x00; // ack no
    // data[29] = 0x00; // ack no
    // data[30] = 0x00; // ack no
    // data[31] = 0x00; // ack no
    memcpy(data + 28, &ackNo, 4);
    data[32] = 0x50;
    data[33] = 0x12; // SYN + ACK
    data[34] = 0xff;
    data[35] = 0xff;
    data[36] = 0x00;//chk sum;
    data[37] = 0x00;//chk sum;
    data[38] = 0x00;
    data[39] = 0x00;

    // 构造伪头部
    uint8_t checksum[IP_MAXPACKET];
    memset(checksum, 0, IP_MAXPACKET);
    memcpy(checksum, data + 12, 4); //源地址
    memcpy(checksum +4, data + 16, 4); //目标地址
    checksum[9] = data[9]; //协议
    checksum[10] = 0; // TCP长度
    checksum[11] = 20; // TCP长度
    memcpy(checksum + 12, data + 20, 20); //数据
    chk = chksum((uint16_t *)checksum, 20+12);
    data[36] = (chk >> 8) & 0xFF;
    data[37] = chk & 0xFF;
    // printf("%02x %02x\n", data[36], data[37]);
    return SEND_LEN(bufflen + ethlen);
}

int mk_buf_send_ack(uint8_t * data_raw, int len,
    uint32_t snNo, uint32_t ackNo,
    const raw_sock_env_conf* gRawSockEnvConf)
{
    int bufflen = 40;
    uint8_t byte2[2];

    snNo = htonl(snNo);
    ackNo = htonl(ackNo);
    uint16_t hostPort = htons(gRawSockEnvConf->host_port);
    uint16_t remotePort = htons(gRawSockEnvConf->remote_port);

    // Destination and Source MAC addresses
    int ethlen = cp_eth_info(data_raw, len, gRawSockEnvConf);

    uint8_t * data = data_raw + ethlen;
    data[0] = 0x45;
    data[1] = 0;
    data[2] = ((uint8_t*)&bufflen)[1];
    data[3] = ((uint8_t*)&bufflen)[0];
    data[4] = 0x00;
    data[5] = 0x00;
    data[6] = 0x00;
    data[7] = 0x00;
    data[8] = 0x40;
    data[9] = 0x06;
    data[10] = 0;//chk sum;
    data[11] = 0;//chk sum;
    memcpy(data + 12, gRawSockEnvConf->host_ip, 4);
    memcpy(data + 16, gRawSockEnvConf->remote_ip, 4);

    uint16_t chk = chksum((uint16_t *)data, 20);
    data[10] = (chk >> 8) & 0xFF;
    data[11] = chk & 0xFF;

    memcpy(byte2, &(hostPort), 2);
    data[20] = byte2[0]; // 17018
    data[21] = byte2[1]; // 17018
    memcpy(byte2, &remotePort, 2);
    data[22] = byte2[0]; // dport
    data[23] = byte2[1]; // dport
    // data[24] = 0x00; // snm
    // data[25] = 0xea; // snm
    // data[26] = 0x0d; // snm
    // data[27] = 0x66; // snm
    memcpy(data + 24, &snNo, 4);
    // data[28] = 0x00; // ack no
    // data[29] = 0x00; // ack no
    // data[30] = 0x00; // ack no
    // data[31] = 0x00; // ack no
    memcpy(data + 28, &ackNo, 4);
    data[32] = 0x50;
    data[33] = 0x10; // ACK
    data[34] = 0xff;
    data[35] = 0xff;
    data[36] = 0x00;//chk sum;
    data[37] = 0x00;//chk sum;
    data[38] = 0x00;
    data[39] = 0x00;

    // 构造伪头部
    uint8_t checksum[IP_MAXPACKET];
    memset(checksum, 0, IP_MAXPACKET);
    memcpy(checksum, data + 12, 4); //源地址
    memcpy(checksum +4, data + 16, 4); //目标地址
    checksum[9] = data[9]; //协议
    checksum[10] = 0; // TCP长度
    checksum[11] = 20; // TCP长度
    memcpy(checksum + 12, data + 20, 20); //数据
    chk = chksum((uint16_t *)checksum, 20+12);
    data[36] = (chk >> 8) & 0xFF;
    data[37] = chk & 0xFF;
    // printf("%02x %02x\n", data[36], data[37]);
    return SEND_LEN(bufflen + ethlen);
}

int mk_buf_send_fin(uint8_t * data_raw, int len,
    uint32_t snNo, uint32_t ackNo,
    const raw_sock_env_conf* gRawSockEnvConf)
{
    int bufflen = 40;
    uint8_t byte2[2];

    snNo = htonl(snNo);
    ackNo = htonl(ackNo);
    uint16_t hostPort = htons(gRawSockEnvConf->host_port);
    uint16_t remotePort = htons(gRawSockEnvConf->remote_port);

    // Destination and Source MAC addresses
    int ethlen = cp_eth_info(data_raw, len, gRawSockEnvConf);

    uint8_t * data = data_raw + ethlen;
    data[0] = 0x45;
    data[1] = 0;
    data[2] = ((uint8_t*)&bufflen)[1];
    data[3] = ((uint8_t*)&bufflen)[0];
    data[4] = 0x00;
    data[5] = 0x00;
    data[6] = 0x00;
    data[7] = 0x00;
    data[8] = 0x40;
    data[9] = 0x06;
    data[10] = 0;//chk sum;
    data[11] = 0;//chk sum;
    memcpy(data + 12, gRawSockEnvConf->host_ip, 4);
    memcpy(data + 16, gRawSockEnvConf->remote_ip, 4);

    uint16_t chk = chksum((uint16_t *)data, 20);
    data[10] = (chk >> 8) & 0xFF;
    data[11] = chk & 0xFF;

    memcpy(byte2, &(hostPort), 2);
    data[20] = byte2[0]; // 17018
    data[21] = byte2[1]; // 17018
    memcpy(byte2, &remotePort, 2);
    data[22] = byte2[0]; // dport
    data[23] = byte2[1]; // dport
    // data[24] = 0x00; // snm
    // data[25] = 0xea; // snm
    // data[26] = 0x0d; // snm
    // data[27] = 0x66; // snm
    memcpy(data + 24, &snNo, 4);
    // data[28] = 0x00; // ack no
    // data[29] = 0x00; // ack no
    // data[30] = 0x00; // ack no
    // data[31] = 0x00; // ack no
    memcpy(data + 28, &ackNo, 4);
    data[32] = 0x50;
    data[33] = 0x11; // ACK + FIN
    data[34] = 0xff;
    data[35] = 0xff;
    data[36] = 0x00;//chk sum;
    data[37] = 0x00;//chk sum;
    data[38] = 0x00;
    data[39] = 0x00;

    // 构造伪头部
    uint8_t checksum[IP_MAXPACKET];
    memset(checksum, 0, IP_MAXPACKET);
    memcpy(checksum, data + 12, 4); //源地址
    memcpy(checksum +4, data + 16, 4); //目标地址
    checksum[9] = data[9]; //协议
    checksum[10] = 0; // TCP长度
    checksum[11] = 20; // TCP长度
    memcpy(checksum + 12, data + 20, 20); //数据
    chk = chksum((uint16_t *)checksum, 20+12);
    data[36] = (chk >> 8) & 0xFF;
    data[37] = chk & 0xFF;
    // printf("%02x %02x\n", data[36], data[37]);
    return SEND_LEN(bufflen + ethlen);
}

int send_row_data(int sd, uint8_t * p, int len,
    const raw_sock_env_conf* gRawSockEnvConf)
{
    int bytes;
    // Fill out sockaddr_ll.
    // gRawSockEnvConf->device.sll_family = AF_PACKET;
    // memcpy (gRawSockEnvConf->device.sll_addr, gRawSockEnvConf->host_mac, IFHWADDRLEN);
    // gRawSockEnvConf->device.sll_halen = htons (IFHWADDRLEN);
    // Send ethernet frame to socket.
    if ((bytes = sendto (sd, p, len, 0, (struct sockaddr *) &(gRawSockEnvConf->device), sizeof (gRawSockEnvConf->device))) <= 0) {
        perror ("sendto() failed");
        exit (EXIT_FAILURE);
    }
    return bytes;
}
