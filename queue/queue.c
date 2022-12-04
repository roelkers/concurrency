#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

static long producersWaiting = 0;
static long consumersWaiting = 0;
static sem_t sem;
static sem_t sem2;
static pthread_mutex_t mutex;

enum {
    N_PRODUCERS = 2,
    N_CONSUMERS = 2 
};

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)

static void producerWait() {
  if(sem_wait(&sem) == -1){
      errExit("producerWait");
  }
}

static void consumerWait() {
  if(sem_wait(&sem2) == -1){
      errExit("consumerWait");
  }
}

static void producerPost() {
  if(sem_post(&sem) == -1){
      errExit("producerPost");
  }
}

static void consumerPost() {
  if(sem_post(&sem2) == -1){
      errExit("consumerPost");
  }
}

static void *producersFunc(void *arg)
{
    long loops = *((long *) arg); 
    for (size_t i = 0; i < loops; i++) {
        printf("producer starting\n");

        pthread_mutex_lock(&mutex);
        if(consumersWaiting > 0) {
          consumersWaiting--;
          consumerPost();
        } else {
          producersWaiting++;
          pthread_mutex_unlock(&mutex);
          producerWait();
        }

        printf("unblocked producer\n");
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

static void *consumersFunc(void *arg)
{
    long loops = *((long *) arg);
    for (size_t i = 0; i < loops; i++) {
        printf("consumer starting\n");

        pthread_mutex_lock(&mutex);
        if(producersWaiting > 0) {
          producersWaiting--;
          producerPost();
        } else {
          consumersWaiting++;
          pthread_mutex_unlock(&mutex);
          consumerWait();
        }
        
        printf("unblocked consumer\n");
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t producers[N_PRODUCERS];
    pthread_t consumers[N_CONSUMERS];
    int s;
    long nloops = 4;

    if(pthread_mutex_init(&mutex, NULL) != 0) {
        printf("\n mutex init failed\n");
        return 1;
    }

    if (sem_init(&sem, 0, 0) == -1)
        errExit("sem_init");
    if (sem_init(&sem2, 0, 0) == -1)
        errExit("sem_init");

    for (int i = 1; i <= N_PRODUCERS; ++i) {
        s = pthread_create(&producers[i], NULL, producersFunc, &nloops);
        if (s != 0)
            errExit("pthread_create");
    }
    for (int i = 1; i <= N_CONSUMERS; ++i) {
        s = pthread_create(&consumers[i], NULL, consumersFunc, &nloops);
        if (s != 0)
            errExit("pthread_create");
    }

    for (int i = 1; i <= N_CONSUMERS; ++i) {
        s = pthread_join(consumers[i], NULL);
        if (s != 0)
            errExit("pthread_join");
    }
    for (int i = 1; i <= N_CONSUMERS; ++i) {
        s = pthread_join(consumers[i], NULL);
        if (s != 0)
            errExit("pthread_join");
    }

    printf("consumersWaiting = %ld\n", consumersWaiting);
    printf("producersWaiting = %ld\n", producersWaiting);

    exit(EXIT_SUCCESS);
}
