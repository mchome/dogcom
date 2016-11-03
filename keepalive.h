#ifndef KEEPALIVE_H_
#define KEEPALIVE_H_

#include <netinet/in.h>

int keepalive_1(int sockfd, struct sockaddr_in addr, unsigned char seed[], unsigned char auth_information[]);
int keepalive_2(int sockfd, struct sockaddr_in addr, unsigned char seed[], int *keepalive_counter, int *first);
void keepalive_2_packetbuilder(unsigned char keepalive_2_packet[], int keepalive_counter, int filepacket, int type);

#endif // KEEPALIVE_H_