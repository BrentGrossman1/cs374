# Producers and Consumers

## Building

Command line:
* `make` to build. An executable called `pc` will be produced.
* `make clean` to clean up all build products.

## Usage

```
./pc <num_producers> <num_consumers> <events_per_producer> <max_outstanding>
```

* `num_producers`: Number of producer threads to create
* `num_consumers`: Number of consumer threads to create
* `events_per_producer`: Number of events each producer will generate
* `max_outstanding`: Maximum number of events that can be in the queue at one time

Example:
```
./pc 2 4 5 2
```

## Files

* `pc.c`: Main source file containing producer/consumer thread logic and main
* `eventbuf.c`: Event buffer implementation (FIFO queue)
* `eventbuf.h`: Event buffer header and usage documentation
* `Makefile`: Build instructions

## Data

A globally shared `eventbuf` (FIFO queue) holds integer events produced by producer
threads and consumed by consumer threads. Three semaphores coordinate access:

* `mutex`: Binary semaphore acting as a mutex to protect the event buffer
* `items`: Counts the number of events currently in the queue (consumers wait on this)
* `spaces`: Counts the number of free slots in the queue (producers wait on this)

A global `producers_done` flag is set by main after all producers have exited,
signaling consumers to exit once the queue is empty.

## Functions

## Functions

* `main()`
  * `sem_open_temp()`: Helper to create and unlink a named semaphore
  * `eventbuf_create()`: Creates the shared event queue
  * `pthread_create()`: Spawns producer and consumer threads
  * `producer()`: Producer thread function
    * `eventbuf_add()`: Adds an event to the shared queue
  * `consumer()`: Consumer thread function
    * `eventbuf_get()`: Removes and processes an event from the shared queue
    * `eventbuf_empty()`: Checks if the queue is empty before exiting
  * `pthread_join()`: Waits for all producers then all consumers
  * `eventbuf_free()`: Frees the event queue

## Notes

* Output order will vary between runs due to thread scheduling
* Consumers exit when `producers_done` is set and the event queue is empty