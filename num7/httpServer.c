#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

// HTTP 요청을 처리하는 함수
void handle_request(int client_sock) {
    char buffer[BUF_SIZE];
    int bytes_received;

    // 클라이언트로부터 요청을 받음
    bytes_received = recv(client_sock, buffer, BUF_SIZE - 1, 0);
    if (bytes_received < 0) {
        perror("recv failed");
        return;
    }

    // 버퍼에 NULL을 추가하여 문자열 종료
    buffer[bytes_received] = '\0';

    // HTTP GET 요청인지 확인
    if (strstr(buffer, "GET / HTTP/1.1") != NULL) {
        // HTTP GET 요청에 대한 응답
        char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Hello, World!</h1>";
        send(client_sock, response, sizeof(response) - 1, 0);
    }
    // HTTP POST 요청인지 확인
    else if (strstr(buffer, "POST / HTTP/1.1") != NULL) {
        char *content_length = strstr(buffer, "Content-Length: ");
        int length = 0;

        if (content_length) {
            // Content-Length 값을 추출하여 본문 길이 구하기
            content_length += 16; // "Content-Length: "의 길이만큼 건너뛰기
            length = atoi(content_length);
        }

        // POST 데이터 읽기
        char post_data[length + 1];
        recv(client_sock, post_data, length, 0);
        post_data[length] = '\0';

        // HTTP POST 요청에 대한 응답
        char response[BUF_SIZE];
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Received POST data:</h1><p>%s</p>", post_data);
        send(client_sock, response, strlen(response), 0);
    }
    else {
        // 404 오류 응답
        char response[] = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<h1>404 Not Found</h1>";
        send(client_sock, response, sizeof(response) - 1, 0);
    }

    // 연결 종료
    close(client_sock);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // 소켓 생성
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(1);
    }

    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // 바인딩
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(1);
    }

    // 리스닝
    if (listen(server_sock, 5) < 0) {
        perror("listen failed");
        exit(1);
    }

    printf("Server running on port %d...\n", PORT);

    // 클라이언트 연결 처리
    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("accept failed");
            continue;
        }

        // 클라이언트 요청 처리
        handle_request(client_sock);
    }

    // 소켓 닫기
    close(server_sock);
    return 0;
}
