//Jakobe McPhail
//Professor Ali
//02/17/26

#include <stdio.h>
#include <pthread.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

//Problem 1.1
/*void* identify(void* arg)
{
    pid_t pid = getpid();
    printf("My pid is %d\n", pid);
    return NULL;
}

int main()
{
    pthread_t thread;
    pthread_create(&thread, NULL, &identify, NULL);
    identify(NULL);
    return 0;
}*/

//Problem 3.1

/*void *helper(void *arg)
{
    printf("HELPER\n");
    return NULL;
}

int main()
{
    pthread_t thread;
    pthread_create(&thread, NULL, &helper, NULL);
    pthread_yield();
    printf("MAIN\n");
    return 0;
}*/

//Problem 3.2

/*void *helper(void *arg)
{
    int *num = (int*) arg;
    *num = 2;
    return NULL;
}

int main()
{
    int i = 0;
    pthread_t thread;
    pthread_create(&thread, NULL, &helper, &i);
    pthread_join(thread, NULL);
    printf("i is %d\n", i);
    return 0;
}*/

//Problem 3.3

/*void *helper(void *arg)
{
    char *message = (char *) arg;
        strcpy(message, "I am the child");
            return NULL;
            }

            int main()
            {
                char *message = malloc(100);
                    strcpy(message, "I am the parent");
                        pthread_t thread;
                            pthread_create(&thread, NULL, &helper, message);
                                pthread_join(thread, NULL);
                                    printf("%s\n", message);
                                        return 0;
            }*/

//Problem 3.4

void *worker(void *arg)
{
    int *data = (int *) arg;
    *data = *data + 1;
    printf("Data is %d\n", *data);
    return (void *) 42;
}

int data;
int main()
{
    int status;
    data = 0;
    pthread_t thread;
    pid_t pid = fork();
    if(pid == 0)
    {
        pthread_create(&thread, NULL, &worker, &data);
        pthread_join(thread, NULL);
    }
    else
    {
        pthread_create(&thread, NULL, &worker, &data);
        pthread_join(thread, NULL);
        pthread_create(&thread, NULL, &worker, &data);
        pthread_join(thread, NULL);
        wait(&status);
    }
    return 0;
}