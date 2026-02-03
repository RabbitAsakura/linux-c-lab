//Jakobe McPhail

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(void)
{
    pid_t pidA = fork();
    if(pidA == 0)
    {
        execl("/bin/echo", "echo", "one", "two", NULL);
        perror("execl failed");
        exit(1);
    }

    pid_t pidB = fork();
    if (pidB == 0)
    {
        char *argv_array[] = {"echo", "one", "two", NULL};
        execv("/bin/echo", argv_array);
        perror("execv failed");
        exit(1);
    }




    wait(NULL);
    wait(NULL);
    return 0;
}