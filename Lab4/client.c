//Jakobe McPhail
//02/20/26
//Professor Ali

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080



int main()
{
    int sock;
    struct sockaddr_in server_addr;
    char buffer[1024];


    sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        return 1;
    }

    strcpy(buffer, "shutdown");
    printf("Sending shutdown\n");
    send(sock, buffer, strlen(buffer), 0);

    char response[1024];
    memset(response, 0, sizeof(response));
    if (read(sock, response, sizeof(response) - 1) > 0)
    {
    printf("Server response: %s\n", response);
    }
    close(sock);
    return 0;
}