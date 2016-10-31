#ifndef AUTH_H_
#define AUTH_H_

#include <netinet/in.h>

int challenge(int sockfd, struct sockaddr_in addr);
int dogcom(int try_times);
void print_packet(char msg[10], unsigned char *packet, int length);

#endif // AUTH_H_