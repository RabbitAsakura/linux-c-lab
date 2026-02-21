//Jakobe McPhail
//02/20/26
//Professor Ali

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080


void handle_client(int client_sock, int server_sock)
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    read(client_sock, buffer, sizeof(buffer) - 1);
    printf("Received: %s\n", buffer);

    write(client_sock, "Hello from server", 17);
    close(client_sock);

    if(strcmp(buffer, "shutdown") == 0)
    {
        printf("Closing server\n");
        kill(getppid(), SIGINT);
        close(client_sock);
        close(server_sock);
        exit(0);
    }
}

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main()
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    int opt = 1;
    
    server_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(EXIT_FAILURE);
    }

    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 5);
    
    signal(SIGCHLD, sigchld_handler);

    while(1)
    {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        if(fork() == 0)
        {
            handle_client(client_sock, server_sock);
            exit(0);
        }
    }
    return 0;
}