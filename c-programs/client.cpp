//Jakobe McPhail
//02/19/26
//Professor Ali


#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

#define PORT 8080

int main()
{
    int sock;
    struct sockaddr_in server_addr;
    char buffer[1024] = "Hello C++ Server";

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    send(sock, buffer, strlen(buffer), 0);
    read(sock, buffer, sizeof(buffer));
    printf("Server response: %s\n", buffer);
    close(sock);
    return 0;
}

//For the other exercises
/*int main(){
    int sock;
    struct sockaddr_in server_addr;
    char buffer[1024];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    printf("Connected to server. Type messages (type 'exit' to quit):\n");

    while (1) {
        printf("> ");
        fgets(buffer, sizeof(buffer), stdin);
        
        buffer[strcspn(buffer, "\n")] = 0;

        send(sock, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        memset(buffer, 0, sizeof(buffer));
        int bytes_received = read(sock, buffer, sizeof(buffer) - 1);
        if (bytes_received > 0) {
            printf("%s\n", buffer);
        } else {
            printf("Server disconnected.\n");
            break;
        }
    }
    close(sock);
    return 0;
}*/