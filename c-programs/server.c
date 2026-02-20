//Jakobe McPhail
//02/19/26
//Professor Ali

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <errno.h>

#define MAX_CLIENTS 10
#define PORT 8080
#define TIMEOUT 10

/*void handle_client(int client_sock)
{
    char buffer[1024];
    read(client_sock, buffer, sizeof(buffer));
    printf("Received: %s\n", buffer);
    write(client_sock, "Hello from server", 17);
    close(client_sock);
}*/



/*int main()
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 5);

    while(1)
    {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        if(fork() == 0)
        {
            handle_client(client_sock);
            exit(0);
        }
    }
    return 0;
}*/


//Exercise 3
/*void handle_client(int client_sock)
{
    char buffer[1024];
    char echo_msg[1100];
    int bytes;

    while(1)
    {
        memset(buffer, 0, sizeof(buffer));
        bytes = read(client_sock, buffer, sizeof(buffer) - 1);
        if(bytes <= 0)
        {
            break;
        }

        buffer[strcspn(buffer, "\n")] = 0;
        printf("Received from client: %s\n", buffer);

        if(strcmp(buffer, "exit") == 0)
        {
            printf("Client requested exit.\n");
            break;
        }

        snprintf(echo_msg, sizeof(echo_msg), "Echo: %s", buffer);
        write(client_sock, echo_msg, strlen(echo_msg));
    }
    close(client_sock);
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
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 5);

    printf("Server listening on port %d...\n", PORT);

    signal(SIGCHLD, SIG_IGN);

    while(1) {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        
        if(fork() == 0) { // Child process
            close(server_sock); 
            handle_client(client_sock);
            exit(0);
        }
        close(client_sock);
    }
    return 0;
}*/


//Exercise 5
/*void handle_timeout(int sig) {
    printf("\n[Timeout] No activity for %d seconds. Terminating child process.\n", TIMEOUT);
    exit(0); 
}

void handle_client(int client_sock) {
    char buffer[1024];
    int bytes_read;
    signal(SIGALRM, handle_timeout);

    while (1) {
        alarm(TIMEOUT);

        memset(buffer, 0, sizeof(buffer));
        bytes_read = read(client_sock, buffer, sizeof(buffer) - 1);
        
        // If we read data, cancel the alarm so the process doesn't exit
        alarm(0);

        if (bytes_read <= 0) {
            printf("Client disconnected or error occurred.\n");
            break;
        }

        buffer[strcspn(buffer, "\n")] = 0;
        printf("Received: %s\n", buffer);

        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        char *response = "Echo: ";
        write(client_sock, response, strlen(response));
        write(client_sock, buffer, strlen(buffer));
    }
    close(client_sock);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 5);
    
    signal(SIGCHLD, SIG_IGN);

    printf("Server waiting for connections (10s timeout enabled)...\n");

    while(1) {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        
        if(fork() == 0) { 
            handle_client(client_sock);
            exit(0);
        }
        close(client_sock);
    }
    return 0;
}*/

//Exercise 7
struct SharedData {
    int client_sockets[MAX_CLIENTS];
    int active_count;
};

void broadcast(struct SharedData* shared_mem, int sender_fd, char* msg) {
    char broadcast_buffer[1100];
    snprintf(broadcast_buffer, sizeof(broadcast_buffer), "Broadcast: %s", msg);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        int target_fd = shared_mem->client_sockets[i];
        // Send to everyone except the sender and empty slots
        if (target_fd > 0 && target_fd != sender_fd) {
            write(target_fd, broadcast_buffer, strlen(broadcast_buffer));
        }
    }
}

void handle_client(int client_sock, struct SharedData* shared_mem) {
    char buffer[1024];
    int my_index = -1;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (shared_mem->client_sockets[i] == 0) {
            shared_mem->client_sockets[i] = client_sock;
            shared_mem->active_count++;
            my_index = i;
            break;
        }
    }

    printf("New client connected. Active: %d\n", shared_mem->active_count);

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = read(client_sock, buffer, sizeof(buffer) - 1);
        
        if (bytes <= 0 || strcmp(buffer, "exit") == 0) break;

        buffer[strcspn(buffer, "\n")] = 0;
        printf("Received to broadcast: %s\n", buffer);

        broadcast(shared_mem, client_sock, buffer);
    }

    if (my_index != -1) {
        shared_mem->client_sockets[my_index] = 0;
        shared_mem->active_count--;
    }
    printf("Client disconnected. Active: %d\n", shared_mem->active_count);
    close(client_sock);
}

int main() {
    int shm_id = shmget(IPC_PRIVATE, sizeof(struct SharedData), IPC_CREAT | 0666);
    struct SharedData* shared_mem = (struct SharedData*)shmat(shm_id, NULL, 0);
    memset(shared_mem, 0, sizeof(struct SharedData));

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {AF_INET, htons(PORT), INADDR_ANY};
    
    bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_sock, 5);
    signal(SIGCHLD, SIG_IGN);

    printf("Broadcast Server started on port %d...\n", PORT);

    while (1) {
        int client_sock = accept(server_sock, NULL, NULL);
        if (fork() == 0) {
            handle_client(client_sock, shared_mem);
            shmdt(shared_mem);
            exit(0);
        }
        close(client_sock); 
    }
    return 0;
}
