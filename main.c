#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include "queue.h"

// DEFAULT NUMBERS OF COMMAND_LINE ARGUMENTS
#define DEFAULT_NUMBER_OF_WORKER_THREADS 3
#define DEFAULT_MAX_QUEUE_SIZE 5
#define DEFAULT_NUM_RANDOMS 10
#define DEFAULT_LOWER_BOUND 1
#define DEFAULT_UPPER_BOUND 100
#define DEFAULT_GENERATION_RATE 100
#define TRUE 1 // for infinite loop
Queue queue;

void *generator_thread(void *args ) {
    //taking the numbers from "generator_args".

    int lower_bound = *(((int *)args));

    int upper_bound = *(((int *)args) +1);
    //maximum available upper bound is 2000.
    if (upper_bound > 2000) {
        upper_bound = 2000;
    }
    int num_randoms = *(((int *)args) +3);

    int generation_rate = *((double *)(((int *)args) +2));
    int i;
    
    for (i = 1; i <= num_randoms; i++) {

        int random_number = rand() % (upper_bound - lower_bound + 1) + lower_bound;
        
        QueueInsert(&queue, random_number);
        

        //sleep time with exponential distribution.
        double x = (double)rand() / RAND_MAX; // Random number between 0 and 1.
        
        double sleep_time = -log(1 - x) / generation_rate; //log is ln.
        
        //sleep for calculated amount.
        usleep((unsigned int)(sleep_time * 1000000)); 
    }
    return NULL;
}
//threads take the numbers in the queue and decide if they are prime numbers.
void *worker_thread(void *args) {

    while (TRUE) {
        int prime_control = 1;
        int counter =0; //counter for maximum 10 divisors.
        int number_in_progress = QueueRemove(&queue); //takes the number from the queue.
        
        //prime number detecting algorithm.
        for (int i = 2; i <= sqrt(number_in_progress); i++ ) {

            if (number_in_progress % i == 0) {

                prime_control = 0;
                break;
            }
        }
        
        if (prime_control) {
            
            printf("Thread ID: %lu, Number: %d is a prime number.\n", pthread_self(), number_in_progress);

        } else {

            printf("Thread ID: %lu, Number: %d is not a prime number. Divisors:", pthread_self(), number_in_progress);

            for (int k = 1; k <= number_in_progress && counter <10; k++ ) {
                if (number_in_progress % k == 0 ) {
                    printf(" %d", k) ;
                    counter ++;
                } 
            }
           printf("\n");
        }
        
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int num_threads = DEFAULT_NUMBER_OF_WORKER_THREADS;//default is 3
    int queue_size =  DEFAULT_MAX_QUEUE_SIZE;//default is 5
    int num_randoms = DEFAULT_NUM_RANDOMS;//default is 10
    int lower_bound = DEFAULT_LOWER_BOUND;//default is 1
    int upper_bound = DEFAULT_UPPER_BOUND;//default is 100
    int generation_rate = DEFAULT_GENERATION_RATE;//default is 100
    
    int opt;
    while ((opt = getopt(argc, argv, "t:q:r:m:n:g:")) != -1) {
        switch (opt) {
            case 't':
                num_threads = atoi(optarg);
                break;

            case 'q':
                queue_size = atoi(optarg);
                break;

            case 'r':
                num_randoms = atoi(optarg);
                break;

            case 'm':
                lower_bound = atoi(optarg);
                break;

            case 'n':
                upper_bound = atoi(optarg);
                break;

            case 'g':
                generation_rate = atof(optarg);
                break;

            default:
                fprintf(stderr, "Usage: %s [-t num_threads] [-q queue_size] [-r num_randoms] [-m lower_bound] [-n upper_bound] [-g generation_rate]\n", argv[0]);

                exit(EXIT_FAILURE);
        }
    }
    
    srand(time(NULL));
    
    QueueInitialize(&queue, queue_size);
    int generator_args[4] = {lower_bound, upper_bound, generation_rate, num_randoms};// for generator thread.
    pthread_t generator;
    pthread_create(&generator, NULL, generator_thread, (void *)generator_args); //creating generator thread according to inputs.
    
    pthread_t worker[num_threads];
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&worker[i], NULL, worker_thread, NULL); //creating worker threads according to the input.
    }
    
    pthread_join(generator, NULL);
    for (int i = 0; i < num_threads; i++) {
        pthread_cancel(worker[i]);
    }
    
    QueueDestroy(&queue);
    
    return 0;
}