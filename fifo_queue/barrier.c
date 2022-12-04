#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

static long head_waiting = 0;
static sem_t sem;
static pthread_mutex_t mutex;

enum {THREADS = 4};

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)

static void *threadFunc(void *arg)
{
    long threadNr = *((long *) arg);

    printf("reached barrier i\n");

    pthread_mutex_lock(&mutex);
    shared++;
    pthread_mutex_unlock(&mutex); 

    if(head_waiting == 1) {
        printf("%li count reached \n", shared);
        printf("count is at max\n");
        if (sem_post(&sem) == -1) {
            errExit("sem_post");
        }
    }

    if (sem_wait(&sem) == -1)
      errExit("sem_wait");

    if (sem_post(&sem) == -1) {
        errExit("sem_post");
    }

    printf("passed parrier \n");
    return NULL;
}

static void signal() {
    if(head_waiting == 1) {
        sem
    if (sem_post(&sem) == -1) {
        errExit("sem_post");
    }
    }
}

static void wait() {

}

int main(int argc, char *argv[]) {
    pthread_t t[THREADS];
    int s;
    long nloops;

    if(pthread_mutex_init(&mutex, NULL) != 0) {
        printf("\n mutex init failed\n");
        return 1;
    }

    if (sem_init(&sem, 0, 0) == -1)
        errExit("sem_init");

    for (int i = 1; i <= THREADS; ++i) {
        s = pthread_create(&t[i], NULL, threadFunc, &i);
        if (s != 0)
            errExit("pthread_create");
    }

    for (int i = 1; i <= THREADS; ++i) {
        s = pthread_join(t[i], NULL);
        if (s != 0)
            errExit("pthread_join");
    }

    printf("shared = %ld\n", shared);
    exit(EXIT_SUCCESS);
}
