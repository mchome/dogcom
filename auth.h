#ifndef AUTH_H_
#define AUTH_H_

#include <netinet/in.h>

int challenge(int sockfd, struct sockaddr_in addr, unsigned char seed[]);
int login(int sockfd, struct sockaddr_in addr, unsigned char seed[], unsigned char auth_information[]);
int dogcom(int try_times);
void print_packet(char msg[10], unsigned char *packet, int length);

#endif // AUTH_H_