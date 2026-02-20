//Jakobe McPhail
//02/19/26
//Professor Ali

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 8080

/*void handle_client(int client_sock)
{
    char buffer[1024];
    read(client_sock, buffer, sizeof(buffer));
    std::cout << "Received: " << buffer << std::endl;
    write(client_sock, "Hello from C++ server", 22);
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

    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 5);

    while(true)
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

//Exercise 4
/*struct SharedData {
    int active_clients;
};

void handle_client(int client_sock, SharedData* shared_mem) {
    char buffer[1024];
    shared_mem->active_clients++;
    std::cout << "New connection! Active clients: " << shared_mem->active_clients << std::endl;

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = read(client_sock, buffer, sizeof(buffer) - 1);
        
        if (bytes_read <= 0 || strcmp(buffer, "exit") == 0) {
            break;
        }
        std::string response = "Echo: " + std::string(buffer);
        write(client_sock, response.c_str(), response.length());
    }
    shared_mem->active_clients--;
    std::cout << "Client disconnected. Active clients: " << shared_mem->active_clients << std::endl;
    close(client_sock);
}

int main() {
    int shm_id = shmget(IPC_PRIVATE, sizeof(SharedData), IPC_CREAT | 0666);
    SharedData* shared_mem = (SharedData*)shmat(shm_id, NULL, 0);
    shared_mem->active_clients = 0;

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 5);
    
    signal(SIGCHLD, SIG_IGN); 

    std::cout << "Server started. Waiting for connections..." << std::endl;

    while (true) {
        sockaddr_in client_addr;
        socklen_t addr_size = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);

        if (fork() == 0) { // Child Process
            handle_client(client_sock, shared_mem);
            shmdt(shared_mem); 
            exit(0);
        }
        close(client_sock); 
    }

    shmdt(shared_mem);
    shmctl(shm_id, IPC_RMID, NULL);
    return 0;
}*/


//Exercise 6
void log_error(const std::string& operation) {
    std::ofstream log_file("server_errors.log", std::ios::app);
    if (log_file.is_open()) {
        std::time_t now = std::time(0);
        char* dt = std::ctime(&now);
        // Remove the trailing newline character from ctime
        dt[std::strlen(dt) - 1] = '\0'; 
        
        log_file << "[" << dt << "] ERROR: " << operation << " failed - " 
                 << std::strerror(errno) << std::endl;
        log_file.close();
    }
}

void handle_client(int client_sock) {
    char buffer[1024];
    while (true) {
        std::memset(buffer, 0, sizeof(buffer));
        int bytes_read = read(client_sock, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0 || std::strcmp(buffer, "exit") == 0) break;

        std::string response = "Echo: " + std::string(buffer);
        write(client_sock, response.c_str(), response.length());
    }
    close(client_sock);
}

int main() {

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        log_error("socket()");
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        log_error("bind()");
        close(server_sock);
        return 1;
    }

    if (listen(server_sock, 5) < 0) {
        log_error("listen()");
        close(server_sock);
        return 1;
    }

    signal(SIGCHLD, SIG_IGN);

    std::cout << "Server running. Check server_errors.log for any issues." << std::endl;

    while (true) {
        sockaddr_in client_addr;
        socklen_t addr_size = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        if (client_sock < 0) {
            log_error("accept()");
            continue; 
        }
        pid_t pid = fork();
        if (pid < 0) {
            log_error("fork()");
            close(client_sock);
        } else if (pid == 0) { // Child process
            close(server_sock);
            handle_client(client_sock);
            exit(0);
        } else { // Parent process
            close(client_sock);
        }
    }
    return 0;
}