# Seat Reservation System with Mutexes

## Building

Command line:
* `make` to build. An executable called `reservations` will be produced.
* `make clean` to remove the executable.

## Files

* `reservations.c`: Main program implementing thread-safe seat reservation system
* `Makefile`: Build configuration

## Data

The main data structures are:

* `seat_taken[]`: Integer array where 1 indicates a seat is reserved, 0 indicates free
* `seat_taken_count`: Global counter tracking total number of reserved seats
* `pthread_mutex_t mutex`: Mutex protecting access to shared seat data

Multiple broker threads compete to reserve and unreserve seats. All access to shared data is protected by a mutex to prevent race conditions.

## Functions

* `main()`
  * Parses command line arguments
  * Allocates seat array and thread structures
  * Launches broker threads
  * Waits for all brokers to complete
  * Frees allocated memory
* `seat_broker()`: Thread function for each broker
  * Performs random reserve/unreserve transactions
  * Validates seat count after each transaction
  * Reports success or failure
* `reserve_seat(int n)`: Reserves seat n
  * Locks mutex before checking/modifying seat data
  * Returns -1 if already taken, 0 on success
  * Increments `seat_taken_count` if successful
* `free_seat(int n)`: Unreserves seat n
  * Locks mutex before checking/modifying seat data
  * Returns -1 if already free, 0 on success
  * Decrements `seat_taken_count` if successful
* `verify_seat_count()`: Validates seat count consistency
  * Locks mutex to read seat data atomically
  * Counts reserved seats and compares to `seat_taken_count`
  * Returns true if counts match, false otherwise

## Usage
```bash
./reservations <seat_count> <broker_count> <transaction_count>
```

Where:
- `<seat_count>` is the number of seats in the venue
- `<broker_count>` is the number of competing broker threads
- `<transaction_count>` is the number of transactions each broker performs

Example:
```bash
./reservations 10 5 100
```

Successful output (all brokers happy):
```
Broker 0: That all seemed to work very well.
Broker 1: That all seemed to work very well.
Broker 2: That all seemed to work very well.
Broker 3: That all seemed to work very well.
Broker 4: That all seemed to work very well.
```

Failed output (race conditions detected):
```
Broker 0: the seat count seems to be off! I quit!
Broker 1: the seat count seems to be off! I quit!
Broker 2: the seat count seems to be off! I quit!
Broker 3: the seat count seems to be off! I quit!
Broker 4: the seat count seems to be off! I quit!
```

## Notes

* Use small numbers first to test (like 10 seats, 5 brokers, 100 transactions)
* The mutex locks prevents race conditions when multiple brokers access the same seats
* Broker output prints in random order due to thread scheduling
* `Ctrl+C` to quit if you accidentally launch too many threads