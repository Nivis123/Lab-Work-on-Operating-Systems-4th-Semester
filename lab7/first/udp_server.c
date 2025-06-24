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
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP Server is listening on port %d...\n", PORT);

    while (1) {
        ssize_t recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                                   (struct sockaddr *)&client_addr, &client_len);
        if (recv_len < 0) {
            perror("recvfrom failed");
            continue;
        }

        buffer[recv_len] = '\0';
        printf("Received from %s:%d: %s\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);

        if (sendto(sockfd, buffer, recv_len, 0,
                  (const struct sockaddr *)&client_addr, client_len) < 0) {
            perror("sendto failed");
        }
    }

    close(sockfd);
    return 0;
}