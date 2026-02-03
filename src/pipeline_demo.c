#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int fd[2];
    if(pipe(fd) == -1) {perror("pipe"); exit(1);}

    pid_t pid1 = fork();
    if(pid1 == 0)
    {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execlp("ls", "ls", (char *)NULL);
        perror("execlp ls");
        exit(1);
    }

    pid_t pid2 = fork();
    if(pid2 == 0)
    {
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        close(fd[0]);

        execlp("grep", "grep", "c", (char *)NULL);
        perror("execlp grep");
        exit(1);
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}