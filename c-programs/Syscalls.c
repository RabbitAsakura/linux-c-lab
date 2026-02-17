//Jakobe McPhail
//Professor Ali
//02/17/26
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>


//3.1 Question

/*void handle(int sig)
{
    char choice;
    write(1, "Do you really want to quit[y/n]?",33);
    
    read(0, &choice, 1);

    if(choice == 'y' || choice == 'Y')
    {
        exit(0);
    }

}*/

/*int main()
{
    signal(SIGINT, handle);
    while(1);
}*/



//4.1 Question

/*int main()
{
    char buffer[200];
    memset(buffer, 'a', 200);
    int fd = open("test.txt", O_CREAT | O_RDWR);
    write(fd, buffer, 200);
    lseek(fd,0,SEEK_SET);
    read(fd, buffer, 100);
    lseek(fd, 500, SEEK_CUR);
    write(fd, buffer, 100);
}*/


//4.2 Question

//4.2.1
/*int main(int argc, char **argv)
{
    int pid, status;
    int newfd;
    if ((newfd = open("output_file.txt", O_CREAT | O_TRUNC | O_WRONLY, 0644)) < 0)
    {
        exit(1);
    }
    printf("Luke, I am your...\n");
    dup2(newfd, 1);
    printf("father\n");
    exit(0);
}*/

//4.2.2 

/*int main(int argc, char **argv)
{
    int pid, status;
    int newfd;
    char* cmd[] = { "/bin/ls", "-al", "/", 0};

    if(argc != 2)
    {
        fprintf(stderr, "usage: %s output_file\n", argv[0]);
        exit(1);
    }

    if((newfd = open(argv[1], O_CREAT | O_TRUNC|O_WRONLY, 0644)) < 0)
    {
        perror(argv[1]);
        exit(1);
    }
    printf("writing output of the command %s to \"%s\"\n", cmd[0], argv[1]);
    dup2(newfd, 1);
    execvp(cmd[0], cmd);
    perror(cmd[0]);
    exit(1);
}*/

//4.2.3

int main(int argc, char **argv)
{
    int pid, status;
    int newfd;

    pid = fork();

        if(argc != 2)
    {
        fprintf(stderr, "usage: %s output_file\n", argv[0]);
        exit(1);
    }

    
    if(pid < 0)
    {
        perror("fork failed");
        exit(1);
    }

    if(pid == 0)
    {
        char* cmd[] = {"/bin/ls", "-al", "/", 0};
        int newfd = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY, 0644);
        if(newfd < 0) { perror(argv[1]); exit(1);}
        dup2(newfd, 1);
        close(newfd);

        execvp(cmd[0], cmd);
        perror("execvp failed");
        exit(1);
    }  
    else
    {
        wait(NULL);
        printf("all done\n");
    }
  
}