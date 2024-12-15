#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 3490
#define MAXDATASIZE 100

int main(int argc, char *argv[]) {
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct sockaddr_in server_addr;

    if (argc != 2) {
        fprintf(stderr, "usage: selectclient <server_ip>\n");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(1);
    }

    sprintf(buf, "Hello, server");
    if (send(sockfd, buf, strlen(buf), 0) == -1) {
        perror("send");
        exit(1);
    }

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    buf[numbytes] = '\0';
    printf("Received: %s\n", buf);

    close(sockfd);
    return 0;
}
