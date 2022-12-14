#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

int readers = 0;
static sem_t roomEmpty;
static sem_t turnstile;
static pthread_mutex_t mutex;
static pthread_mutex_t mutex;


enum {
    N_READERS = 4,
    N_WRITERS = 4,
};

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)

static void roomPost() {
  if(sem_post(&roomEmpty) == -1){
      errExit("producerPost");
  }
}

static void roomWait() {
  if(sem_wait(&roomEmpty) == -1){
      errExit("consumerPost");
  }
}

static void turnstilePost() {
  if(sem_post(&turnstile) == -1){
      errExit("producerPost");
  }
}

static void turnstileWait() {
  if(sem_wait(&turnstile) == -1){
      errExit("consumerPost");
  }
}

static void *writersFunc(void *arg)
{
    long loops = *((long *) arg);
    for (size_t i = 0; i < loops; i++) {
        printf("producer starting\n");

        turnstileWait();
        roomWait();
        
        // Critical section
        //printf("readers %i \n", readers);

        turnstilePost();
        roomPost();
    }

    return NULL;
}

static void *readersFunc(void *arg)
{
    long loops = *((long *) arg);
    for (size_t i = 0; i < loops; i++) {
        printf("consumer starting\n");

        turnstileWait();
        turnstilePost();
        
        pthread_mutex_lock(&mutex);
        readers++;
        if(readers == 1) {
            roomWait();
        }
        pthread_mutex_unlock(&mutex);

        // Critical section

        pthread_mutex_lock(&mutex);
        readers--;
        if(readers == 0) {
            roomPost();
        }
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t readers[N_READERS];
    pthread_t writers[N_WRITERS];
    int s;
    long nloops = 1000;

    if(pthread_mutex_init(&mutex, NULL) != 0) {
        printf("\n mutex init failed\n");
        return 1;
    }

    if (sem_init(&roomEmpty, 0, 1) == -1)
        errExit("sem_init");
    if (sem_init(&turnstile, 0, 1) == -1)
        errExit("sem_init");
    
    for (int i = 1; i <= N_WRITERS; ++i) {
        printf("init writers %i\n", i);
        s = pthread_create(&writers[i], NULL, writersFunc, &nloops);
        if (s != 0)
            errExit("pthread_create");
    }
    for (int i = 1; i <= N_READERS; ++i) {
        s = pthread_create(&readers[i], NULL, readersFunc, &nloops);
        if (s != 0)
            errExit("pthread_create");
    }

    for (int i = 1; i <= N_READERS; ++i) {
        s = pthread_join(readers[i], NULL);
        if (s != 0)
            errExit("pthread_join");
    }
    for (int i = 1; i <= N_WRITERS; ++i) {
        s = pthread_join(writers[i], NULL);
        if (s != 0)
            errExit("pthread_join");
    }

    printf("All threads exited \n");
    /* printf("consumersWaiting = %ld\n", consumersWaiting); */
    /* printf("producersWaiting = %ld\n", producersWaiting); */

    exit(EXIT_SUCCESS);
}
