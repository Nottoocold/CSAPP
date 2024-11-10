#include "../csapp.h"
#include "pthread.h"
#include "semaphore.h"

volatile long cnt = 0;
sem_t mutex;

void *worker(void *arg);

int main(int argc, char *argv[])
{
    long niters;
    pthread_t t1, t2;

    if (argc != 2)
    {
        printf("Usage: %s <number of iterations>\n", argv[0]);
        exit(0);
    }

    niters = atol(argv[1]);

    sem_init(&mutex, 0, 1); // initialize mutex

    pthread_create(&t1, NULL, worker, &niters);
    pthread_create(&t2, NULL, worker, &niters);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    if (cnt != 2 * niters)
    {
        printf("Error: cnt = %ld, expected %ld\n", cnt, 2 * niters);
        exit(0);
    }
    printf("cnt = %ld\n", cnt);
    return 0;
}

void *worker(void *arg)
{
    long i, niters = *((long *)arg);

    for (i = 0; i < niters; i++)
    {
        sem_wait(&mutex); // lock
        cnt++; // load , update, store
        sem_post(&mutex); // unlock
    }

    return NULL;
}