#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }
    
    const char *ip = argv[1];
    int port = atoi(argv[2]);
    
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);
    
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sockfd);
        exit(1);
    }
    
    printf("Connected to server at %s:%d\n", ip, port);
    printf("Type 'quit' to exit\n");
    
    while (1) {
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        
        if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
            perror("send");
            break;
        }
        
        if (strncmp(buffer, "quit", 4) == 0) {
            break;
        }
        
        ssize_t bytes_received = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            perror("recv");
            break;
        }
        
        buffer[bytes_received] = '\0';
        printf("Server echo: %s", buffer);
    }
    
    close(sockfd);
    printf("Connection closed\n");
    return 0;
}