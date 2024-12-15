#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT            3490
#define MAX_CLNT        100
#define BUF_SIZE        512

int client_count = 0;
int client_socks[MAX_CLNT];
fd_set allfds;  // 전역 변수로 선언

void send_msg_to_all(char *msg, int len);
void handle_client(int client_sock);

int main() {
    int server_sock, client_sock, max_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    fd_set readfds;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket() failed");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind() failed");
        exit(1);
    }

    if (listen(server_sock, MAX_CLNT) < 0) {
        perror("listen() failed");
        exit(1);
    }

    FD_ZERO(&allfds);
    FD_SET(server_sock, &allfds);
    max_sock = server_sock;

    printf("Server is running...\n");

    while (1) {
        readfds = allfds;
        if (select(max_sock + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("select() failed");
            exit(1);
        }

        for (int i = 0; i <= max_sock; i++) {
            if (FD_ISSET(i, &readfds)) {
                if (i == server_sock) {
                    client_len = sizeof(client_addr);
                    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
                    if (client_sock < 0) {
                        perror("accept() failed");
                        continue;
                    }

                    FD_SET(client_sock, &allfds);
                    if (client_sock > max_sock) {
                        max_sock = client_sock;
                    }

                    client_socks[client_count++] = client_sock;
                    printf("New client connected: %d\n", client_sock);
                } else {
                    handle_client(i);
                }
            }
        }
    }

    close(server_sock);
    return 0;
}

void handle_client(int client_sock) {
    int str_len;
    char msg[BUF_SIZE];

    str_len = read(client_sock, msg, BUF_SIZE);
    if (str_len == 0) {
        close(client_sock);
        FD_CLR(client_sock, &allfds);
        printf("Client disconnected: %d\n", client_sock);
        return;
    } else if (str_len == -1) {
        perror("read() failed");
        return;
    }

    msg[str_len] = '\0';
    send_msg_to_all(msg, str_len);
}

void send_msg_to_all(char *msg, int len) {
    for (int i = 0; i < client_count; i++) {
        write(client_socks[i], msg, len);
    }
}
