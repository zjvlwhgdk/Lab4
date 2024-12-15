#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char request[BUF_SIZE];
    char response[BUF_SIZE];
    
    // 서버 소켓 주소 설정
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // localhost
    server_addr.sin_port = htons(PORT);

    // 서버에 연결
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect failed");
        exit(1);
    }

    // HTTP GET 요청 보내기
    sprintf(request, "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
    send(sock, request, strlen(request), 0);

    // 서버의 응답 받기
    recv(sock, response, BUF_SIZE - 1, 0);
    printf("Response from server:\n%s\n", response);

    // HTTP POST 요청 보내기
    sprintf(request, "POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 13\r\n\r\nHello, Server!");
    send(sock, request, strlen(request), 0);

    // 서버의 응답 받기
    recv(sock, response, BUF_SIZE - 1, 0);
    printf("Response from server:\n%s\n", response);

    // 소켓 종료
    close(sock);
    return 0;
}
