#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

static int create_and_bind(char *port);
static int make_socket_non_binding(int sfd);


#define MAX_EVENTS 64
#define BUF_SIZE   20
#define MAX_WORKER 20

static int
create_and_bind(char *port) {
    struct addrinfo hint, *result;
    int res, sfd;
  
    
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_family   = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags    = AI_PASSIVE;
    
    res = getaddrinfo(NULL, port, &hint, &result);
    if (res == -1) {
        perror("error : can not get address info\n");
        exit(1);
    }
    
    sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sfd == -1) {
        perror("error : can not get socket descriptor!\n");
        exit(1);
    }
    
    res = bind(sfd, result->ai_addr, result->ai_addrlen);
    if (res == -1) {
        perror("error : can not bind the socket!\n");
        exit(1);
    }
    
    freeaddrinfo(result);
    
    return sfd;
}


static int
make_socket_non_binding(int sfd)
{
    int flags, res;
    
    flags = fcntl(sfd, F_GETFL);
    if (flags == -1) {
        perror("error : cannot get socket flags!\n");
        exit(1);
    }
    
    flags |= O_NONBLOCK;
    res    = fcntl(sfd, F_SETFL, flags);
    if (res == -1) {
        perror("error : cannot set socket flags!\n");
        exit(1);
    }
    
    return 0;
}

int
main(int argc, char *argv[])
{
    int sfd, res, epoll, cnt, i, sd;
    struct epoll_event event, events[MAX_EVENTS];
    pid_t pid;
    
    
    sfd = create_and_bind("6666");
    if (sfd == -1) {
        perror("error : cannot create socket!\n");
        exit(1);
    }
    
    res = make_socket_non_binding(sfd);
    if (res == -1) {
        perror("error : connot set flags!\n");
        exit(1);
    }
    
    res = listen(sfd, SOMAXCONN);
    if (res == -1) {
        perror("error : cannot listen!\n");
        exit(1);
    }
    
    epoll = epoll_create(1);
    if (epoll == -1) {
        perror("error : cannot create epoll!\n");
        exit(1);
    }
    
    event.events  = EPOLLIN | EPOLLOUT | EPOLLET;
    event.data.fd = sfd;
    res = epoll_ctl(epoll, EPOLL_CTL_ADD, sfd, &event);
    if (res == -1) {
        perror("error : can not add event to epoll!\n");
        exit(1);
    }
    
    
    for (i = 0; i < MAX_WORKER; i++) {
        if ((pid = fork()) < 0) {
            perror("error\n");
            
        } else if (pid == 0) {
            while (1) {
                cnt = epoll_wait(epoll, events, MAX_EVENTS, -1);
        
                for (i = 0; i < cnt; i++) {
            
                    if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)
                        || !(events[i].events & EPOLLIN))
                    {
                        perror("error : socket fd error!\n");
                        close(events[i].data.fd);
                        continue;
            
                    } else if (events[i].data.fd == sfd) {
                
                        while (1) { 
                            struct sockaddr client_addr;
                            int addrlen = sizeof(struct sockaddr);
                    
                            sd = accept(sfd, &client_addr, &addrlen);
                            if (sd == -1) {
                    
                                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                                    if (errno == EAGAIN) {
                                        perror("\n jing qun\n\n");
                                    }
                                    break;
                        
                                } else {
                                    perror("error : cannot accept new socket!\n");
                                    continue;
                                }
                        
                            } 
                    
                            res = make_socket_non_binding(sd);
                            if (res == -1) {
                                perror("error : cannot set flags!\n");
                                exit(1);
                            }
                    
                            event.data.fd = sd;
                            event.events  = EPOLLET | EPOLLIN;
                            res = epoll_ctl(epoll, EPOLL_CTL_ADD, sd, &event);
                            if (res == -1) {
                                perror("error : cannot add to epoll!\n");
                                exit(1);
                            }
                        }  
                
                    } else {
                        int cnt, active;
                        char buf[BUF_SIZE];
                        
                       active = 1;
                        while (1) {
                
                            cnt = read(events[i].data.fd,  buf, BUF_SIZE);
                            if (cnt == -1) {
                                if (errno == EAGAIN) {
                                    break;
                                }
                        
                                printf("error : read error, error code = %d\n", errno);
                                exit(1);
                
                            } else if (cnt == 0) {
                                close(events[i].data.fd);
                                active = 0;
                                perror("client closed the connect\n");
                                break;
                            } 
                
                            printf("receive client data : %s\n", buf);
                        }
                        
                        if (active) {
                            memset(buf, 0, BUF_SIZE);
                            strcpy(buf, "Hello Client!");
                            write(events[i].data.fd, buf, strlen(buf));
                            printf("send < Hello Client > to client\n");
                            
                        }
                    }
                }
            }
        }
    }
    
    while (waitpid(-1, NULL, 0));
    close(sfd);
    
    return 0;
}
