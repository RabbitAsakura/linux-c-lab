//Jakobe McPhail

#include <stdio.h>
#include <unistd.h>

int main()
{
    pid_t pid1 = fork();

    if(pid1 > 0)
    {
        printf("I am the parent, PID: %d\n", getpid());
    }
    else if(pid1 == 0)
    {
        printf("I am the child, PID: %d\n", getpid());
        pid_t pid2 = fork();
        if(pid2 == 0)
        {
             printf("I am the grandchild, PID: %d\n", getpid());
        }
    }
    return 0;
}