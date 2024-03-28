#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>

typedef struct {
    int *array;
    int size;
    int current_size;
    int back;
    int front;
    

    pthread_mutex_t mutex;
    
    pthread_cond_t not_empty;

    pthread_cond_t available;
    
} Queue;

void QueueInitialize(Queue *queue, int size);

void QueueInsert(Queue *queue, int value);

int QueueRemove(Queue *queue);

void QueueDestroy(Queue *queue);

#endif 
