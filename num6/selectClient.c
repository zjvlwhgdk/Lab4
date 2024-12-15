#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT            3490
#define BUF_SIZE        512

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server_addr;
    char msg[BUF_SIZE];

    if (argc != 2) {
        printf("Usage: %s <server_ip>\n", argv[0]);
        exit(1);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket() failed");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect() failed");
        exit(1);
    }

    printf("Connected to server...\n");

    while (1) {
        printf("Enter message: ");
        fgets(msg, BUF_SIZE, stdin);
        msg[strlen(msg) - 1] = '\0';

        if (strcmp(msg, "quit") == 0) {
            break;
        }

        if (send(sock, msg, strlen(msg), 0) == -1) {
            perror("send() failed");
            exit(1);
        }

        int str_len = recv(sock, msg, BUF_SIZE, 0);
        if (str_len == -1) {
            perror("recv() failed");
            exit(1);
        }
        msg[str_len] = '\0';

        printf("Received from server: %s\n", msg);
    }

    close(sock);
    return 0;
}
