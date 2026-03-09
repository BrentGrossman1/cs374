/*

Brent Grossman
03/08/2026
cs374
Project 9 - Producer and Consumers

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

#include "eventbuf.h"

// semaphores 
sem_t *mutex;       // protects the event buffer from simultaneous access
sem_t *items;       // counts how many events are in the queue (consumers wait on this)
sem_t *spaces;      // counts how many free slots exist in the queue (producers wait on this)

// global event buffer
struct eventbuf *eb;

// global done flag, set by main after all producers finish
int producers_done = 0;

// global command line args so threads can see them
int num_producers;
int num_consumers;
int events_per_producer;
int max_outstanding;


sem_t *sem_open_temp(const char *name, unsigned int value)
{
    sem_t *sem;

    // Create the semaphore
    if ((sem = sem_open(name, O_CREAT, 0600, value)) == SEM_FAILED)
        return SEM_FAILED;
    // Unlink it so it will go away after this process exits
    if (sem_unlink(name) == -1) {
        sem_close(sem);
        return SEM_FAILED;
    }

    return sem;
}

// producer thread
void *producer(void *arg)
{
    int thread_id = *(int *)arg;

    for (int i = 0; i < events_per_producer; i++) {
        // compute the event number using the formula:
        //      producer_number * 100 + event_number
        int event = thread_id * 100 + i;

        // sem_wait(spaces) to wait for a free slot
        sem_wait(spaces);

        // sem_wait(mutex) to lock the queue
        sem_wait(mutex);

        // eventbuf_add the event
        eventbuf_add(eb, event);

        // print the producer thread number and which event its adding to the eventbuf Queue
        printf("P%d: adding event %d\n", thread_id, event);

        // sem_post(mutex) to unlock the queue
        sem_post(mutex);

        // sem_post(items) to signal consumers that something is ready
        sem_post(items);
    }

    printf("P%d: exiting\n", thread_id);

    return NULL;
}

// consumer thread
void *consumer(void *arg)
{
    int thread_id = *(int *)arg;

    while (1) {
        // sem_wait(items) to wait for something to consume
        sem_wait(items);

        // check if producers_done is set AND eventbuf_empty, if so break
        if (producers_done && eventbuf_empty(eb)) {
            break;
        }

        // sem_wait(mutex) to lock the queue
        sem_wait(mutex);

        // print the consumer thread number and which event its consuming from the eventbuf Queue
        printf("C%d: got event %d\n", thread_id, eventbuf_get(eb));

        // sem_post(mutex) to unlock the queue
        sem_post(mutex);

        // sem_post(spaces) to signal producers that a slot freed up
        sem_post(spaces);
    }

    printf("C%d: exiting\n", thread_id);

    return NULL;
}

int main(int argc, char *argv[])
{
    // check that argc == 5, print usage and exit if not
    if (argc != 5) {
        printf("Usage: ./pc <int_1> <int_2> <int_3> <int_4>\n");
        printf("int_1: The number of producers\n"
               "int_2: The number of consumers\n"
               "int_3: The number of events each producer will generate\n"
               "int_4: The number of outstanding events there can be at a time.\n");
        exit(1);
    }

    // parse the 4 command line args into their global variables
    num_producers = atoi(argv[1]);
    num_consumers = atoi(argv[2]);
    events_per_producer = atoi(argv[3]);
    max_outstanding = atoi(argv[4]);

    // create the event buffer 
    eb = eventbuf_create();

    // initialize the three semaphores using sem_open_temp:
    //    mutex  -> initial value 1
    //    items  -> initial value 0
    //    spaces -> initial value max_outstanding
    mutex = sem_open_temp("sem_mutex_lock", 1);
    items = sem_open_temp("sem_items", 0);
    spaces = sem_open_temp("sem_spaces", max_outstanding);

    // create producer threads
    pthread_t producer_threads[num_producers];
    int producer_thread_id[num_producers];

    // loop and pthread_create each producer, passing its ID
    for (int i = 0; i < num_producers; i++) {
        producer_thread_id[i] = i;
        pthread_create(producer_threads + i, NULL, producer, producer_thread_id + i);
    }

    // create consumer threads
    pthread_t consumer_threads[num_consumers];
    int consumer_thread_id[num_consumers];

    // loop and pthread_create each consumer, passing its ID
    for (int i = 0; i < num_consumers; i++) {
        consumer_thread_id[i] = i;
        pthread_create(consumer_threads + i, NULL, consumer, consumer_thread_id + i);
    }

    // wait for all producers to finish
    for (int i = 0; i < num_producers; i++) {
        pthread_join(producer_threads[i], NULL);
    }

    // signal consumers that producers are done
    producers_done = 1;
    for (int i = 0; i < num_consumers; i++) {
        sem_post(items);
    }

    // wait for all consumers to finish
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(consumer_threads[i], NULL);
    }

    // cleanup 
    sem_close(mutex);
    sem_close(items);
    sem_close(spaces);
    eventbuf_free(eb);

    return 0;
}