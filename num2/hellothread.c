#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *hello_thread(void *arg)
{
    printf("Thread: Hello, World!\n");
    return arg;
}

int main()
{
    pthread_t tid;
    int status;

    status = pthread_create(&tid, NULL, hello_thread, NULL);
    if (status != 0)
    {
        perror("Create thread");
        exit(1);
    }

    pthread_exit(NULL);
}