#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid = fork();

    if(pid < 0)
    {
        perror("fork");
        exit(1);
    }

    if(pid == 0)
    {
        printf("Child (PID: %d) \n", getpid());
        exit(0);
    }
    else
    {
        printf("Parent (PID: %d) sleep for 20 seconds\n");
        sleep(20);

        printf("Parent wait\n");
        wait(NULL);

        printf("Zombie\n");
        sleep(5);
    }
    return 0;
}