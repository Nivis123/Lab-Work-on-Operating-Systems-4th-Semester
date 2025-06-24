#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

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
    struct pollfd fds[MAX_CLIENTS + 1]; 
    int nfds = 1; 
    char buffer[BUFFER_SIZE];


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

    fds[0].fd = server_socket;
    fds[0].events = POLLIN;

    while (1) {
        int ret = poll(fds, nfds, -1); 
        if (ret < 0) {
            perror("poll");
            break;
        }

  
        for (int i = 0; i < nfds; i++) {
            if (fds[i].revents == 0)
                continue;

            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == server_socket) {
                    client_len = sizeof(client_addr);
                    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
                    if (client_socket < 0) 
                        {perror("accept");
                        continue;
                    }

                    printf("New connection from %s:%d\n", 
                          inet_ntoa(client_addr.sin_addr), 
                          ntohs(client_addr.sin_port));

                    if (nfds < MAX_CLIENTS + 1) {
                        fds[nfds].fd = client_socket;
                        fds[nfds].events = POLLIN;
                        nfds++;
                    } else {
                        printf("Too many clients, connection refused\n");
                        close(client_socket);
                    }
                } 
                
                else {
                    ssize_t bytes_read = recv(fds[i].fd, buffer, BUFFER_SIZE - 1, 0);
                    if (bytes_read <= 0) {
                        
                        printf("Client disconnected\n");
                        close(fds[i].fd);
                        
                        for (int j = i; j < nfds - 1; j++) {
                            fds[j] = fds[j + 1];
                        }
                        i--; 
                        nfds--;
                    } else {
                        buffer[bytes_read] = '\0';
                        printf("Received: %s", buffer);
                        
                        send(fds[i].fd, buffer, bytes_read, 0);
                        
                        if (strncmp(buffer, "quit", 4) == 0) {
                            printf("Client requested disconnect\n");
                            close(fds[i].fd);
                            for (int j = i; j < nfds - 1; j++) {
                                fds[j] = fds[j + 1];
                            }
                            i--;
                            nfds--;
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < nfds; i++) {
        close(fds[i].fd);
    }

    return 0;
}