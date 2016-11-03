#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "libs/md4.h"
#include "libs/md5.h"
// #include "libs/sha1.h"
#include "keepalive.h"
#include "configparse.h"
#include "auth.h"
#include "debug.h"

int keepalive_1(int sockfd, struct sockaddr_in addr, unsigned char seed[], unsigned char auth_information[]) {
    unsigned char keepalive_1_packet[42], recv_packet[100], MD5A[16];
    memset(keepalive_1_packet, 0, 42);
    keepalive_1_packet[0] = 0xff;
    int MD5A_len = 6 + strlen(drcom_config.password);
    unsigned char MD5A_str[MD5A_len];
    MD5A_str[0] = 0x03;
    MD5A_str[1] = 0x01;
    memcpy(MD5A_str + 2, seed, 4);
    memcpy(MD5A_str + 6, drcom_config.password, strlen(drcom_config.password));
    MD5(MD5A_str, MD5A_len, MD5A);
    memcpy(keepalive_1_packet + 1, MD5A, 16);
    memcpy(keepalive_1_packet + 20, auth_information, 16);
    keepalive_1_packet[36] = rand() & 0xff;
    keepalive_1_packet[37] = rand() & 0xff;

    sendto(sockfd, keepalive_1_packet, 42, 0, (struct sockaddr *)&addr, sizeof(addr));
    
    if (verbose_flag) {
        print_packet("[Keepalive1 sent] ", keepalive_1_packet, 42);
    }

#ifdef TEST
    printf("[TEST MODE]IN TEST MODE, PASS");
    return 0;
#endif

    socklen_t addrlen = sizeof(addr);
    if (recvfrom(sockfd, recv_packet, 1024, 0, (struct sockaddr *)&addr, &addrlen) < 0) {
        perror("Failed to recv data");
        return 1;
    }
    if (recv_packet[0] != 0x07) {
        printf("Bad keepalive1 response received.\n");
        return 1;
    }

    if (verbose_flag) {
        print_packet("[Keepalive1 recv] ", recv_packet, sizeof(recv_packet));
    }

    return 0;
}

void keepalive_2_packetbuilder(unsigned char keepalive_2_packet[], int filemode){
    keep_alive2_packet[0] = 0x07;
}

int keepalive_2(int sockfd, struct sockaddr_in addr, unsigned char seed[], unsigned char auth_information[], int keepalive_counter) {
    unsigned char keepalive_2_packet[40];
    keepalive_2_packetbuilder(keepalive_2_packet);
    return 0;
}