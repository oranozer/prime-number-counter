#include "queue.h"
#include <stdlib.h>

//creating the queue.
void QueueInitialize(Queue *queue, int size) {
    
    queue->array = (int *) malloc (size * sizeof(int));
    queue->size = size;
    queue->current_size = 0;
    queue->front = 0 ;
    queue->back = -1 ;
    //initialization of the thread and conditions.
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->available, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
}
//puting numbers inside of the queue
void QueueInsert(Queue *queue, int value) {
    pthread_mutex_lock(&queue->mutex); // gaining access.
    while (queue->current_size >= queue->size) {

        pthread_cond_wait(&queue->available, &queue->mutex); // wait if the queue is full.
    }
    queue->back = (queue->back + 1) % queue->size; //shift the queue

    queue->array[queue->back] = value; //instert the value to the most back of the queue.
    queue->current_size++; 
    pthread_cond_signal(&queue->not_empty); // it is no more empty
    pthread_mutex_unlock(&queue->mutex);// gives the access.
}

//taking numbers inside of the queue(removing the used ones.).

int QueueRemove(Queue *queue) {

    pthread_mutex_lock(&queue->mutex); // gaining access.
    while (queue->current_size <= 0) { // queue should be not-empty.

        pthread_cond_wait(&queue->not_empty, &queue->mutex);
    }
    int value = queue->array[queue->front]; //worker takes the first value of the queue.
    queue->front = (queue->front + 1) % queue->size; //shift the queue.
    queue->current_size--;

    pthread_cond_signal(&queue->available); // it is not full.
    pthread_mutex_unlock(&queue->mutex); // gives the access.

    return value; //returns the taken number from the queue.
}
// frees the memory and destroy the threads.
void QueueDestroy(Queue *queue) {

    free(queue->array);
    
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->available);
    pthread_cond_destroy(&queue->not_empty);
}