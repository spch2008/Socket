#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

int main(int argc, char *argv[])
{
    struct addrinfo hint, *result;
    struct sockaddr remote;
    int res, sfd, fd;
    char buf[100];

    memset(&hint, 0, sizeof(hint));
    hint.ai_family   = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = 0;
    hint.ai_flags    = AI_PASSIVE;

    res = getaddrinfo(NULL, "8080", &hint, &result);
    if (res != 0) {
        perror("error : cannot get socket address!\n");
        exit(1);
    }

    sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sfd == -1) {
        perror("error : cannot get socket file descriptor!\n");
        exit(1);
    }
    
    res = bind(sfd, result->ai_addr, result->ai_addrlen);
    if (res == -1) {
        perror("error : cannot bind the socket with the given address!\n");
        exit(1);
    }

    res = listen(sfd, SOMAXCONN);
    if (res == -1) {
        perror("error : cannot listen at the given socket!\n");
        exit(1);
    }

   
    while (1) {
        int len = sizeof(struct sockaddr);
        fd = accept(sfd, &remote, &len);

        res = read(fd, buf, sizeof(buf));
        printf("user data : %s\n", buf);

        /* 模拟 阻塞 处理过程 */
        sleep(10);        

        strcpy(buf, "Hello Client");
        res = write(fd, buf, sizeof(buf));
        printf("send data : %s\n", buf);
    }
    
}
