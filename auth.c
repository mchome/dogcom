#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "libs/md4.h"
#include "libs/md5.h"
#include "libs/sha1.h"
#include "auth.h"
#include "configparse.h"
#include "debug.h"

#define BIND_IP "0.0.0.0"
#define BIND_PORT 61440
#define DEST_PORT 61440

int challenge(int sockfd, struct sockaddr_in addr) {
    unsigned char challenge_packet[20], recv_packet[76];
    memset(challenge_packet, 0, 20);
    challenge_packet[0] = 0x01;
    challenge_packet[1] = 0x02;
    challenge_packet[2] = rand() & 0xff;
    challenge_packet[3] = rand() & 0xff;
    challenge_packet[4] = drcom_config.AUTH_VERSION[0];

    sendto(sockfd, challenge_packet, 20, 0, (struct sockaddr *)&addr, sizeof(addr));
    
    if (verbose_flag) {
        print_packet("[Challenge sent]", challenge_packet, 20);
    }

    socklen_t addrlen = sizeof(addr);
    if (recvfrom(sockfd, recv_packet, 1024, 0, (struct sockaddr *)&addr, &addrlen) < 0) {
        perror("Failed to recv data.\n");
        return 1;
    }

    if (verbose_flag) {
        print_packet("[Challenge recv]", recv_packet, 76);
    }

    return 0;
}

int dogcom(int try_times) {
    int sockfd;

    struct sockaddr_in bind_addr;
    memset(&bind_addr, 0, sizeof(bind_addr));
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = inet_addr(BIND_IP);
    bind_addr.sin_port = htons(BIND_PORT);

    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(drcom_config.server);
    dest_addr.sin_port = htons(DEST_PORT);

    srand(time(NULL));

    // create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("Failed to create socket.\n");
        return 1;
    }
    // bind socket
    if (bind(sockfd, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) < 0) {
        perror("Failed to bind socket.\n");
        return 1;
    }

    // set timeout
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        perror("Failed to set sock opt.\n");
        return 1;
    }

    // start dogcoming
    for(int i = 0; i < try_times; i++) {
        if (!challenge(sockfd, dest_addr)) {
            break;
        }
        sleep(3);
    }

    close(sockfd);
    return 0;
}

void print_packet(char msg[10], unsigned char *packet, int length) {
    printf("%s: ", msg);
    for (int i = 0; i < length; i++) {
        printf("%02x", packet[i]);
    }
    printf("\n");
}