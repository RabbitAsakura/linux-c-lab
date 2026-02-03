//Jakobe McPhail

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid = fork();
    if(pid < 0) {perror("fork"); exit(1);}

    if(pid == 0)
    {
        printf("Hello from Child (PID: %d, PPID=%d) \n", getpid(), getppid());
        sleep(2);
        exit(0);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);

        if(WIFEXITED(status))
        {
            int exit_code = WEXITSTATUS(status);
            printf("child %d exited with status %d\n", pid, exit_code);
        }
    }
    return 0;
}