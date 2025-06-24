#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define PORT 8080

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    printf("UDP Client is ready. Type a message and press Enter...\n");

    while (1) {
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; 

        if (sendto(sockfd, buffer, strlen(buffer), 0,
                  (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("sendto failed");
            continue;
        }

        ssize_t recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (recv_len < 0) {
            perror("recvfrom failed");
            continue;
        }

        buffer[recv_len] = '\0';
        printf("Server replied: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
//gcc udp_server.c -o server && ./server
//gcc udp_client.c -o client && ./client