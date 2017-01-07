#ifndef AUTH_H_
#define AUTH_H_

#ifdef WIN32
    #include <winsock2.h>
#else
    #include <netinet/in.h>
#endif

int challenge(int sockfd, struct sockaddr_in addr, unsigned char seed[]);
int login(int sockfd, struct sockaddr_in addr, unsigned char seed[], unsigned char auth_information[]);
int pppoe_challenge(int sockfd, struct sockaddr_in addr, int *pppoe_counter, unsigned char seed[], unsigned char sip[], int *encrypt_mode);
int pppoe_login(int sockfd, struct sockaddr_in addr, int *pppoe_counter, unsigned char seed[], unsigned char sip[], int *first, int *encrypt_mode, int *encrypt_type);
int dogcom(int try_times);
void print_packet(char msg[10], unsigned char *packet, int length);
void logging(char msg[10], unsigned char *packet, int length);

#endif // AUTH_H_