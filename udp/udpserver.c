#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

#define BUF_SIZE 10


int
main(int argc, char *argv[])
{
    struct addrinfo hint, *result;
    struct sockaddr_in client_addr;
    int res, sfd, client_addr_len;
    char buf[BUF_SIZE];
    
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_family   = AF_INET;
    hint.ai_socktype = SOCK_DGRAM;
    
    res = getaddrinfo(NULL, "8080", &hint, &result);
    if (res == -1) {
        perror("getaddrinfo error!\n");
        exit(1);
    }
    
    sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sfd == -1) {
        perror("socket error!\n");
        exit(1);
    }
    
    res = bind(sfd, result->ai_addr, result->ai_addrlen);
    if (res == -1) {
        perror("bind error!\n");
        exit(1);
    }
    
    while (1) {
        printf("waiting for client ...\n");
        
        res = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr*)&client_addr, &client_addr_len);
        if (res == -1) {
            perror("recvfrom error\n");
            exit(1);
        }
        printf("revceived data : %s\n", buf);
           
        snprintf(buf, BUF_SIZE, "hello client! Total reveive : %d.", res);
        res = sendto(sfd, buf, strlen(buf), 0, (struct sockaddr*)&client_addr, client_addr_len);
        if (res != strlen(buf) ) {
            perror("sendto error\n");
            exit(1);
        }
        printf("send data < %s > to client\n\n", buf);
    }
}
