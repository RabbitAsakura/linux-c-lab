//Jakobe McPhail

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "Usage: %s number of children\n", argv[0]);
        return 1;
    }
    
    int n = atoi(argv[1]);

    for(int i = 0; i < n; i++)
    {
        pid_t pid = fork();
        if(pid < 0) {perror("fork"); exit(1);}

        if(pid == 0)
        {
            printf("Child %d (PID: %d) started\n", i, getpid());
            exit(i + 1);
        }
    }

    printf("Parent waiting\n");
    for(int i = 0; i < n; i++)
    {
        int status;
        pid_t finished = wait(&status);
        if(WIFEXITED(status))
        {
            printf("Child %d finished with status %d\n", finished, WEXITSTATUS(status));
        }
    }
    return 0;
}