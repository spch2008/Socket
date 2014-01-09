#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    struct addrinfo hint, *result;
    int res, sfd;
    char buf[20];

    memset(&hint, 0, sizeof(hint));
    hint.ai_family   = AF_INET;
    hint.ai_socktype = SOCK_STREAM;

    res = getaddrinfo("127.0.0.1", "8080", &hint, &result);
    if (res == -1) {
        perror("error : cannot get socket address!\n");
        exit(1);
    }

    sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sfd == -1) {
        perror("error : cannot get socket file descriptor!\n");
        exit(1);
    }

    res = connect(sfd, result->ai_addr, result->ai_addrlen);
    if (res == -1) {
        perror("error : cannot connect the server!\n");
        exit(1);
    }
    
    strcpy(buf, "Hello Server!");
    write(sfd, buf, sizeof(buf));
    printf("write < %s > to server\n", buf);

    read(sfd, buf, sizeof(buf));
    printf("read < %s > from server\n", buf);

    return 0;
}
