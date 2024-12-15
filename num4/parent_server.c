#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define NUM_CLIENTS 3  // 클라이언트 수

// 전역 변수
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // 뮤텍스
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;    // 조건 변수

char message[256];  // 서버에서 방송할 메시지
int message_ready = 0;  // 메시지가 준비되었는지 여부 (0: 준비 안됨, 1: 준비됨)

// 클라이언트 쓰레드 함수
void* client_thread(void* arg) {
    int client_id = *((int*)arg);

    while (1) {
        pthread_mutex_lock(&mutex);  // 메시지를 기다리기 전에 뮤텍스 잠금

        // 메시지가 준비될 때까지 대기
        while (!message_ready) {
            pthread_cond_wait(&cond, &mutex);
        }

        // 메시지 수신
        printf("Client %d received message: %s\n", client_id, message);

        // 메시지가 전송되었으므로 대기 상태를 해제
        message_ready = 0;  // 메시지를 받은 후, 다시 대기 상태로
        pthread_mutex_unlock(&mutex);  // 뮤텍스 잠금 해제
    }

    return NULL;
}

// 서버(부모) 쓰레드 함수
void* server_thread(void* arg) {
    char input[256];
    
    while (1) {
        // 사용자로부터 메시지 입력 받기
        printf("Enter a message to parent: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';  // 개행문자 제거

        pthread_mutex_lock(&mutex);  // 뮤텍스 잠금

        // 서버가 전송할 메시지 설정
        strncpy(message, input, sizeof(message) - 1);
        message_ready = 1;  // 메시지가 준비되었음을 표시

        // 모든 클라이언트에게 메시지 방송
        pthread_cond_broadcast(&cond);  // 모든 대기 중인 클라이언트에게 신호 보내기

        pthread_mutex_unlock(&mutex);  // 뮤텍스 잠금 해제
    }

    return NULL;
}

int main() {
    pthread_t server;  // 서버 쓰레드
    pthread_t clients[NUM_CLIENTS];  // 클라이언트 쓰레드들
    int client_ids[NUM_CLIENTS];  // 클라이언트 식별자

    // 서버 쓰레드 생성
    pthread_create(&server, NULL, server_thread, NULL);

    // 클라이언트 쓰레드 생성
    for (int i = 0; i < NUM_CLIENTS; i++) {
        client_ids[i] = i + 1;  // 클라이언트 ID (1부터 시작)
        pthread_create(&clients[i], NULL, client_thread, &client_ids[i]);
    }

    // 클라이언트 쓰레드들이 종료될 때까지 기다림
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(clients[i], NULL);
    }

    // 서버 쓰레드 종료
    pthread_join(server, NULL);

    // 자원 해제
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
