#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#define NUM_THREADS 3

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sigset_t sigset;
int completed;

void *signal_thread(void *arg)
{
    int signal;
    int count = 0;
    while (1)
    {
        sigwait(&sigset, &signal);
        if (signal == SIGINT)
        {
            printf("Signal thread: SIGINT %d\n", ++count);
            if (count >= 3)
            {
                pthread_mutex_lock(&mutex);
                completed = 1;
                pthread_mutex_unlock(&mutex);
                break;
            }
        }
    }
    return arg;
}

int main(int argc, char *argv[])
{
    pthread_t tid;
    int arg;
    void *result;
    int status;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: signalthread time(sec)\n");
        exit(1);
    }

    arg = atoi(argv[1]);

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);
    status = pthread_sigmask(SIG_BLOCK, &sigset, NULL);
    if (status != 0)
    {
        fprintf(stderr, "Set signal mask failed.\n");
        exit(1);
    }

    /* Thread creation */
    status = pthread_create(&tid, NULL, signal_thread, NULL);
    if (status != 0)
    {
        fprintf(stderr, "Create thread failed: %d\n", status);
        exit(1);
    }

    while (1)
    {
        sleep(arg);
        pthread_mutex_lock(&mutex);
        printf("Main thread: mutex locked\n");
        if (completed)
            break;
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_unlock(&mutex);
    pthread_exit(result);
}
