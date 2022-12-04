#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

static long head_waiting = 0;
static long group_waiting = 0;
static sem_t sem;
static sem_t sem2;
static pthread_mutex_t mutex;
static pthread_mutex_t mutex_head;

enum {
    N_PRODUCERS = 2,
    N_CONSUMERS = 2 
};

#define errExit(msg) do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)

static void semaphoreWait() {
  if(sem_wait(&sem) == -1){
      errExit("producerWait");
  }
}

static void semaphorePost() {
  if(sem_post(&sem) == -1){
      errExit("producerPost");
  }
}

static void signal() {
    printf("thread will be unlocked");
    if(head_waiting == 1) {
        pthread_mutex_unlock(&mutex);
        pthread_mutex_lock(&mutex_head);
        head_waiting == 0;
        pthread_mutex_unlock(&mutex_head);
        printf("unlocked head\n");
    } else {
       semaphorePost();
    } 
}

static void wait() {
    printf("thread will get locked");
    if(head_waiting == 0) {
       pthread_mutex_lock(&mutex_head);
       head_waiting == 1;
       pthread_mutex_unlock(&mutex_head);
       pthread_mutex_lock(&mutex);
       printf("locked head\n");
    } else {
       semaphoreWait();         
       pthread_mutex_lock(&mutex);
    }
}

static void *producersFunc(void *arg)
{
    long loops = *((long *) arg); 

    for (size_t i = 0; i < loops; i++) {
        printf("producer signalling\n");
       signal(); 
    }

    return NULL;
}

static void *consumersFunc(void *arg)
{
    long loops = *((long *) arg); 

    for (size_t i = 0; i < loops; i++) {
        wait();    
    }

    return NULL;
}


int main(int argc, char *argv[]) {
    pthread_t producers[N_PRODUCERS];
    pthread_t consumers[N_CONSUMERS];
    int s;
    long nloops = 10;

    if(pthread_mutex_init(&mutex, NULL) != 0) {
        printf("\n mutex init failed\n");
        return 1;
    }
    if(pthread_mutex_init(&mutex_head, NULL) != 0) {
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
    for (int i = 1; i <= N_PRODUCERS; ++i) {
        s = pthread_join(producers[i], NULL);
        if (s != 0)
            errExit("pthread_join");
    }
    for (int i = 1; i <= N_CONSUMERS; ++i) {
        s = pthread_join(consumers[i], NULL);
        if (s != 0)
            errExit("pthread_join");
    }

    exit(EXIT_SUCCESS);
}
