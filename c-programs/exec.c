//Jakobe McPhail

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t pid = fork();

    if(pid == 0)
    {
        printf("Executing ls command...\n");
        execlp("ls", "ls", "-l", NULL);
    }
    else
    {
        wait(NULL);
        printf("Parent process finished.\n");
    }
    return 0;
}