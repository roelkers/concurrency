#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

static long shared = 0;
static sem_t sem;
static sem_t sem2;
static pthread_mutex_t mutex;

enum {THREADS = 4};

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)

static void *threadFunc(void *arg)
{
    long loops = *((long *) arg);

    for(int i = 0; i < loops; i++) {
        printf("reached barrier i\n");

        pthread_mutex_lock(&mutex);
        shared++;
        if(shared == THREADS) {
            printf("%li count reached \n", shared);
            printf("count is at max\n");
            //lock second, unlock first
            if (sem_post(&sem) == -1) 
                errExit("sem_post");
            if (sem_wait(&sem2) == -1) 
              errExit("sem_post");
        }
        pthread_mutex_unlock(&mutex); 

        if (sem_wait(&sem) == -1)
          errExit("sem_wait");

        if (sem_post(&sem) == -1) 
          errExit("sem_post");

        //critical point
        printf("passed barrier \n");

        pthread_mutex_lock(&mutex);
        shared--;
        if(shared == 0) {
            printf("%li count reached 0 \n", shared);
            printf("count is at min\n");
            //relock first, open second
            if (sem_post(&sem2) == -1) {
                errExit("sem2_post");
            }
            if (sem_wait(&sem) == -1)
              errExit("sem2_wait");
        }
        pthread_mutex_unlock(&mutex); 

        if (sem_wait(&sem2) == -1)
          errExit("sem2_wait");
        if (sem_post(&sem2) == -1) 
          errExit("sem2_post");
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t t[THREADS];
    int s;
    long nloops = 4;

    if(pthread_mutex_init(&mutex, NULL) != 0) {
        printf("\n mutex init failed\n");
        return 1;
    }

    if (sem_init(&sem, 0, 0) == -1)
        errExit("sem_init");
    if (sem_init(&sem2, 0, 1) == -1)
        errExit("sem_init");

    for (int i = 1; i <= THREADS; ++i) {
        s = pthread_create(&t[i], NULL, threadFunc, &nloops);
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
