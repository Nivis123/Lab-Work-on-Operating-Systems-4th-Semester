#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    
    while ((bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';
        printf("Received: %s", buffer);
        
        send(client_socket, buffer, bytes_read, 0);
        
        if (strncmp(buffer, "quit", 4) == 0) {
            break;
        }
    }
    
    close(client_socket);
    printf("Client disconnected\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }
    
    const char *ip = argv[1];
    int port = atoi(argv[2]);
    
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    pid_t pid;
    
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket");
        exit(1);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);
    
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_socket);
        exit(1);
    }
    
    if (listen(server_socket, 5) < 0) {
        perror("listen");
        close(server_socket);
        exit(1);
    }
    
    printf("Server listening on %s:%d\n", ip, port);
    
    signal(SIGCHLD, SIG_IGN);
    
    while (1) {
        client_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("accept");
            continue;
        }
        
        printf("New connection from %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        pid = fork();
        if (pid < 0) {
            perror("fork");
            close(client_socket);
            continue;
        }
        
        if (pid == 0) { 
            close(server_socket); 
            handle_client(client_socket);
            return 0;
        } else { 
            close(client_socket); 
        }
    }
    
    close(server_socket);
    return 0;
}