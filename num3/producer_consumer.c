#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5  // 버퍼 크기
#define NUM_PRODUCERS 3  // 생산자 수
#define NUM_CONSUMERS 3  // 소비자 수

// 버퍼 구조체
int buffer[BUFFER_SIZE];
int in = 0;  // 생산자가 데이터를 넣을 위치
int out = 0; // 소비자가 데이터를 꺼낼 위치

// 세마포어와 뮤텍스
sem_t empty;  // 빈 공간 세마포어
sem_t full;   // 가득 찬 공간 세마포어
pthread_mutex_t mutex;  // 뮤텍스

// 생산자 쓰레드 함수
void* producer(void* arg) {
    int id = *((int*)arg);
    while (1) {
        // 생산자가 버퍼에 추가할 데이터 생성 (예시로 숫자)
        int item = rand() % 100;
        
        sem_wait(&empty);  // 빈 공간이 있을 때까지 대기
        pthread_mutex_lock(&mutex);  // 버퍼에 접근할 때 뮤텍스 잠금
        
        // 버퍼에 데이터 추가
        buffer[in] = item;
        printf("Producer %d produced item %d at index %d\n", id, item, in);
        in = (in + 1) % BUFFER_SIZE;  // 원형 버퍼로 동작
        
        pthread_mutex_unlock(&mutex);  // 뮤텍스 잠금 해제
        sem_post(&full);  // 가득 찬 공간 세마포어 증가

        // 잠시 대기 (CPU를 다른 쓰레드에 양보)
        sleep(rand() % 3);
    }
}

// 소비자 쓰레드 함수
void* consumer(void* arg) {
    int id = *((int*)arg);
    while (1) {
        sem_wait(&full);  // 데이터가 있을 때까지 대기
        pthread_mutex_lock(&mutex);  // 버퍼에 접근할 때 뮤텍스 잠금
        
        // 버퍼에서 데이터 소비
        int item = buffer[out];
        printf("Consumer %d consumed item %d from index %d\n", id, item, out);
        out = (out + 1) % BUFFER_SIZE;  // 원형 버퍼로 동작
        
        pthread_mutex_unlock(&mutex);  // 뮤텍스 잠금 해제
        sem_post(&empty);  // 빈 공간 세마포어 증가

        // 잠시 대기 (CPU를 다른 쓰레드에 양보)
        sleep(rand() % 3);
    }
}

int main() {
    srand(time(NULL));

    // 세마포어와 뮤텍스 초기화
    sem_init(&empty, 0, BUFFER_SIZE);  // 버퍼 크기만큼 빈 공간이 있음
    sem_init(&full, 0, 0);  // 처음에는 소비자가 소비할 데이터가 없음
    pthread_mutex_init(&mutex, NULL);  // 뮤텍스 초기화

    // 생산자와 소비자 쓰레드 배열
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];
    
    int producer_ids[NUM_PRODUCERS];
    int consumer_ids[NUM_CONSUMERS];

    // 생산자 쓰레드 생성
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producer_ids[i] = i + 1;
        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }

    // 소비자 쓰레드 생성
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        consumer_ids[i] = i + 1;
        pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
    }

    // 쓰레드 종료를 기다림
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }
    
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    // 세마포어와 뮤텍스 자원 해제
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    return 0;
}
