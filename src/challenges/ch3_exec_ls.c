//Jakobe McPhail

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
        printf("Child (PID: %d) is about to run ls -la\n", getpid());

        execlp("ls", "ls", "-la", NULL);
        perror("execlp failed");
        exit(1);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        printf("\nParent: Child process has finished. Goodbye!\n");
    }
}