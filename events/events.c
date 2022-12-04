#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

static int bufferPos = 0;
static char buffer[10] = "hellohello"; 
static sem_t sem;
static sem_t sem2;
static pthread_mutex_t mutex;

enum {
    N_PRODUCERS = 2,
    N_CONSUMERS = 2,
    MAX_BUF_LENGTH = 10
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

/* static void insertNewBuffer(int loop, int prodNr, char buf[10]) { */
/*    char current; */
/*    sprintf(char, "%d", prodNr); */ 
/*    buf[bufferPos] = current; */
/* } */

static void *producersFunc(void *arg)
{
    int prodNr = *((int *) arg); 
    char current;
    for (size_t i = 0; i < 4; i++) {

        pthread_mutex_lock(&mutex);
        if(bufferPos == MAX_BUF_LENGTH) {
          pthread_mutex_unlock(&mutex);
          producerWait();
        }
        bufferPos--;
        //current = itoa(prodNr); 

        buffer[bufferPos] = 'A';
        pthread_mutex_unlock(&mutex);
        consumerPost();
    }

    return NULL;
}

static void *consumersFunc(void *arg)
{
    long loops = *((long *) arg);
    char buffer_copy;
    for (size_t i = 0; i < loops; i++) {
        printf("consumer starting\n");

        pthread_mutex_lock(&mutex);
        if(bufferPos == 0) {
          pthread_mutex_unlock(&mutex);
          consumerWait();
        }
        bufferPos--;
        buffer_copy = buffer[bufferPos];
        pthread_mutex_unlock(&mutex);
        producerPost();
        
        printf("consumer reads bufcontents %c\n", buffer_copy);
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
        s = pthread_create(&producers[i], NULL, producersFunc, &i);
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

    /* printf("consumersWaiting = %ld\n", consumersWaiting); */
    /* printf("producersWaiting = %ld\n", producersWaiting); */

    exit(EXIT_SUCCESS);
}
