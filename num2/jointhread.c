#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void *join_thread(void *arg)
{
    pthread_exit(arg);
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    int arg, status;
    void *result;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: jointhread <number>\n");
        exit(1);
    }

    arg = atoi(argv[1]);

    status = pthread_create(&tid, NULL, join_thread, (void *)arg);
    if (status != 0)
    {
        fprintf(stderr, "Create thread: %d\n", status);
        exit(1);
    }

    status = pthread_join(tid, &result);
    if (status != 0)
    {
        fprintf(stderr, "Join thread: %d\n", status);
        exit(1);
    }

    return (int)result;
}
