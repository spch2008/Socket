#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int
main(int argc, char *argv[])
{
    pid_t pid;

    if ((pid = fork()) < 0) {
        perror("fork error\n");

    } else if (pid == 0) {
        if ((pid = fork()) < 0) {
            perror("fork error\n");

        } else if (pid > 0) {
             printf("first child, parent pid = %d\n", getppid());
             exit(0);

        }

        sleep(2);
        printf("second child, parent pid = %d\n", getppid());
        exit(0);

    }

    if (waitpid(pid, NULL, 0) != pid) {
        perror("wait error\n");

    }
    printf("main exit, pid = %d\n", getpid());
    exit(0);
}
