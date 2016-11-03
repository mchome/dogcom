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
// #include "libs/sha1.h"
#include "auth.h"
#include "configparse.h"
#include "keepalive.h"
#include "debug.h"

#define BIND_IP "0.0.0.0"
#define BIND_PORT 61440
#define DEST_PORT 61440

int challenge(int sockfd, struct sockaddr_in addr, unsigned char seed[]) {
    unsigned char challenge_packet[20], recv_packet[76];
    memset(challenge_packet, 0, 20);
    challenge_packet[0] = 0x01;
    challenge_packet[1] = 0x02;
    challenge_packet[2] = rand() & 0xff;
    challenge_packet[3] = rand() & 0xff;
    challenge_packet[4] = drcom_config.AUTH_VERSION[0];

    sendto(sockfd, challenge_packet, 20, 0, (struct sockaddr *)&addr, sizeof(addr));
    
    if (verbose_flag) {
        print_packet("[Challenge sent] ", challenge_packet, 20);
    }
#ifdef TEST
    unsigned char test[4] = {0x52, 0x6c, 0xe4, 0x00};
    memcpy(seed, test, 4);
    print_packet("[TEST MODE]<PREP SEED> ", seed, 4);
    return 0;
#endif

    socklen_t addrlen = sizeof(addr);
    if (recvfrom(sockfd, recv_packet, 1024, 0, (struct sockaddr *)&addr, &addrlen) < 0) {
        perror("Failed to recv data");
        return 1;
    }
    if (recv_packet[0] != 0x02) {
        printf("Bad challenge response received.\n");
        return 1;
    }

    if (verbose_flag) {
        print_packet("[Challenge recv] ", recv_packet, 76);
    }

    memcpy(seed, &recv_packet[4], 4 * sizeof(*recv_packet));
#ifdef DEBUG
    print_packet("<GET SEED> ", seed, 4);
#endif

    return 0;
}


int login(int sockfd, struct sockaddr_in addr, unsigned char seed[], unsigned char auth_information[]) {
    int login_packet_size;
    if (drcom_config.ror_version) {
        login_packet_size = 338;
    } else {
        login_packet_size = 330;
    }
    unsigned char login_packet[login_packet_size], recv_packet[100], MD5A[16], MACxorMD5A[6], MD5B[16], checksum1[8], checksum2[4];
    memset(login_packet, 0, login_packet_size);
    memset(recv_packet, 0, 100);

    // build login-packet
    login_packet[0] = 0x03;
    login_packet[1] = 0x01;
    login_packet[2] = 0x00;
    login_packet[3] = strlen(drcom_config.username) + 20;
    int MD5A_len = 6 + strlen(drcom_config.password);
    unsigned char MD5A_str[MD5A_len];
    MD5A_str[0] = 0x03;
    MD5A_str[1] = 0x01;
    memcpy(MD5A_str + 2, seed, 4);
    memcpy(MD5A_str + 6, drcom_config.password, strlen(drcom_config.password));
    MD5(MD5A_str, MD5A_len, MD5A);
    memcpy(login_packet + 4, MD5A, 16);
    memcpy(login_packet + 20, drcom_config.username, strlen(drcom_config.username));
    memcpy(login_packet + 56, &drcom_config.CONTROLCHECKSTATUS, 1);
    memcpy(login_packet + 57, &drcom_config.ADAPTERNUM, 1);
    unsigned long int sum = 0;
    unsigned long int mac = 0;
    // pack
    for (int i = 0; i < 6; i++) {
        sum = (int)MD5A[i] + sum * 256;
    }
    // pack
    for (int i = 0; i < 6; i++) {
        mac = (int)drcom_config.mac[i] + mac * 256;
    }
    sum ^= mac;
    // unpack
    for (int i = 6; i > 0; i--) {
        MACxorMD5A[i - 1] = (unsigned char)(sum % 256);
        sum /= 256;
    }
    memcpy(login_packet + 58, MACxorMD5A, sizeof(MACxorMD5A));
    int MD5B_len = 9 + strlen(drcom_config.password);
    unsigned char MD5B_str[MD5B_len];
    memset(MD5B_str, 0, MD5B_len);
    MD5B_str[0] = 0x01;
    memcpy(MD5B_str + 1, drcom_config.password, strlen(drcom_config.password));
    memcpy(MD5B_str + strlen(drcom_config.password) + 1, seed, 4);
    MD5(MD5B_str, MD5B_len, MD5B);
    memcpy(login_packet + 64, MD5B, 16);
    login_packet[80] = 0x01;
    unsigned char host_ip[4];
    sscanf(drcom_config.host_ip, "%hhd.%hhd.%hhd.%hhd",
           &host_ip[0],
           &host_ip[1],
           &host_ip[2],
           &host_ip[3]);
    memcpy(login_packet + 81, host_ip, 4);
    unsigned char checksum1_str[101], checksum1_tmp[4] = {0x14, 0x00, 0x07, 0x0b};
    memcpy(checksum1_str, login_packet, 97);
    memcpy(checksum1_str + 97, checksum1_tmp, 4);
    MD5(checksum1_str, 101, checksum1);
    memcpy(login_packet + 97, checksum1, 8);
    memcpy(login_packet + 105, &drcom_config.IPDOG, 1);
    memcpy(login_packet + 110, &drcom_config.host_name, strlen(drcom_config.host_name));
    unsigned char PRIMARY_DNS[4];
    sscanf(drcom_config.PRIMARY_DNS, "%hhd.%hhd.%hhd.%hhd",
           &PRIMARY_DNS[0],
           &PRIMARY_DNS[1],
           &PRIMARY_DNS[2],
           &PRIMARY_DNS[3]);
    memcpy(login_packet + 142, PRIMARY_DNS, 4);
    unsigned char dhcp_server[4];
    sscanf(drcom_config.dhcp_server, "%hhd.%hhd.%hhd.%hhd",
           &dhcp_server[0],
           &dhcp_server[1],
           &dhcp_server[2],
           &dhcp_server[3]);
    memcpy(login_packet + 146, dhcp_server, 4);
    unsigned char OSVersionInfoSize[4] = {0x94};
    unsigned char OSMajor[4] = {0x05};
    unsigned char OSMinor[4] = {0x01};
    unsigned char OSBuild[4] = {0x28, 0x0a};
    unsigned char PlatformID[4] = {0x02};
    memcpy(login_packet + 162, OSVersionInfoSize, 4);
    memcpy(login_packet + 166, OSMajor, 4);
    memcpy(login_packet + 170, OSMinor, 4);
    memcpy(login_packet + 174, OSBuild, 4);
    memcpy(login_packet + 178, PlatformID, 4);
    memcpy(login_packet + 182, &drcom_config.host_os, strlen(drcom_config.host_os));
    memcpy(login_packet + 310, drcom_config.AUTH_VERSION, 2);
    int counter = 312;
    if (drcom_config.ror_version) {
        login_packet[counter + 1] = strlen(drcom_config.password);
        for(int i = 0, x = 0; i < strlen(drcom_config.password); i++) {
            x = (int)MD5A[i] ^ (int)drcom_config.password[i];
            login_packet[counter + 2 + i] = (unsigned char)(((x << 3) & 0xff) + (x >> 5));
            counter += 1;
        }
        counter += 2;
        // print_packet("TEST ", ror, 6);
    }
    login_packet[counter] = 0x02;
    login_packet[counter + 1] = 0x0c;
    unsigned char checksum2_str[counter + 14];
    unsigned char checksum2_tmp[6] = {0x01, 0x26, 0x07, 0x11};
    memcpy(checksum2_str, login_packet, counter + 2);
    memcpy(checksum2_str + counter + 2, checksum2_tmp, 6);
    memcpy(checksum2_str + counter + 8, drcom_config.mac, 6);
    sum = 1234;
    unsigned long int ret = 0;
    for (int i = 0; i < counter + 14; i += 4) {
        ret = 0;
        // reverse unsigned char array[4]
        for(int j = 4; j > 0; j--) {
            ret = ret * 256 + (int)checksum2_str[i + j - 1];
        }
        sum ^= ret;
    }
    sum = (1968 * sum) & 0xffffffff;
    for (int j = 0; j < 4; j++) {
        checksum2[j] = (unsigned char)(sum >> (j * 8) & 0xff);
    }
    memcpy(login_packet + counter + 2, checksum2, 4);
    memcpy(login_packet + counter + 8, drcom_config.mac, 6);
    login_packet[counter + 16] = 0xe9;
    login_packet[counter + 17] = 0x13;

    sendto(sockfd, login_packet, sizeof(login_packet), 0, (struct sockaddr *)&addr, sizeof(addr));
    
    if (verbose_flag) {
        print_packet("[Login sent] ", login_packet, sizeof(login_packet));
    }

#ifdef TEST
    unsigned char test[16] = {0x44, 0x72, 0x63, 0x6f, 0x77, 0x27, 0x20, 0xca, 0xed, 0x05, 0x6e, 0x35, 0xaa, 0x8b, 0x01, 0xfb};
    memcpy(auth_information, test, 16);
    print_packet("[TEST MODE]<PREP AUTH_INFORMATION> ", auth_information, 16);
    return 0;
#endif

    socklen_t addrlen = sizeof(addr);
    if (recvfrom(sockfd, recv_packet, 1024, 0, (struct sockaddr *)&addr, &addrlen) < 0) {
        perror("Failed to recv data");
        return 1;
    }
    if (recv_packet[0] != 0x04) {
        printf("<<< login failed >>>\n");
        return 1;
    }

    if (verbose_flag) {
        print_packet("[login recv] ", recv_packet, sizeof(recv_packet));
        printf("<<< Loged in >>>\n");
    }
    
    memcpy(auth_information, &recv_packet[23], 16);
#ifdef DEBUG
    print_packet("<GET AUTH_INFORMATION> ", auth_information, 16);
#endif

    if(recvfrom(sockfd, recv_packet, 1024, 0, (struct sockaddr *)&addr, &addrlen) >= 0) {
        DEBUG_PRINT(("Get notice packet."));
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
        perror("Failed to create socket");
        return 1;
    }
    // bind socket
    if (bind(sockfd, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) < 0) {
        perror("Failed to bind socket");
        return 1;
    }

    // set timeout
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        perror("Failed to set sock opt");
        return 1;
    }

    // start dogcoming
    for(int i = 0; i < try_times; i++) {
        unsigned char seed[4];
        unsigned char auth_information[16];
        if (challenge(sockfd, dest_addr, seed)) {
            printf("Retrying...\n");
            sleep(3);
        } else {
            if(!login(sockfd, dest_addr, seed, auth_information)) {
                int keepalive_counter = 0;
                int first = 1;
                while (1) {
                    if(!keepalive_1(sockfd, dest_addr, seed, auth_information)) {
                        if(keepalive_2(sockfd, dest_addr, seed, &keepalive_counter, &first)) {
                            continue;
                        }
                        printf("Keepalive in loop.\n");
                        sleep(20);
                    } else {
                        continue;
                    }
                }
            } else {
                printf("Retrying...\n");
                sleep(3);
            };
        }
    }
    printf(">>>>> Failed to keep in touch with server, exiting <<<<<\n\n");
    close(sockfd);
    return 1;
}


void print_packet(char msg[10], unsigned char *packet, int length) {
    printf("%s", msg);
    for (int i = 0; i < length; i++) {
        printf("%02x", packet[i]);
    }
    printf("\n");
}