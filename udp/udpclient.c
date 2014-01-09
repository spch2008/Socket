#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 100


int
main(int argc, char *argv[])
{
    struct addrinfo hint, *result;
    int res, sfd;
    char buf[BUF_SIZE];
    
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_family   = AF_INET;
    hint.ai_socktype = SOCK_DGRAM;
    
    res = getaddrinfo("127.0.0.1", "8080", &hint, &result);
    if (res == -1) {
        perror("getaddrinfo error!\n");
        exit(1);
    }
    
    sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sfd == -1) {
        perror("socket error!\n");
        exit(1);
    }
    
        
    snprintf(buf, BUF_SIZE, "hello server, now i will send data to you");
    sendto(sfd, buf, strlen(buf), 0, result->ai_addr, result->ai_addrlen);
    printf("send data to clint: %s\n", buf);    
    
    memset(buf, 0, sizeof(buf));
    res = recvfrom(sfd, buf, BUF_SIZE, 0, NULL, NULL);    
    printf("reveive data from server : %s\n", buf);
    
    return 0;
}
