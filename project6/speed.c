
/*
Brent Grossman
02/15/2026
cs374
Project 6 - Speed Controller
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>

#define BUFFER_SIZE 128

// global flags set by signal_handler
volatile sig_atomic_t sigusr1_happened;
volatile sig_atomic_t sigusr2_happened;
// global variable for speed
int speed = 0;

void signal_handler(int sig) {
    (void)sig;
    if (sig == SIGUSR1) {
        sigusr1_happened = 1;
    }
    else if (sig == SIGUSR2) {
        sigusr2_happened = 1;
    }
}

int main(void) {
    // gets the process ID, and prints it to the terminal
    pid_t PID = getpid();
    // buffer to store the data
    char buff[BUFFER_SIZE];

    // block SIGUSR1 and SIGUSR2
    sigset_t mask;
    sigemptyset(&mask);         // create a signal set &mask
    sigaddset(&mask, SIGUSR1);  // add SIGUSR1 to the set
    sigaddset(&mask, SIGUSR2);  // add SIGUSR2 to the set
    sigprocmask(SIG_BLOCK, &mask, NULL);    // use sigprocmask() to block SIGUSR1 and SIGUSR2

    // struct to setup the signal handler
    struct sigaction sa = {
        .sa_handler = signal_handler,
        .sa_flags = SA_RESTART,
    };
    sigemptyset(&sa.sa_mask);

    // call sigaction() to handle SIGUSR1 and SIGUSR2
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    // create an empty sigmask for pselect
    sigset_t zero;
    sigemptyset(&zero);
    
    // set up fd_set for stdin
    fd_set readfds;

    printf("PID=%d\n", PID);

    // main loop that runs until the user enters 'q' to update the running flag to 0
    int running = 1;
    while (running == 1) {
        FD_ZERO(&readfds);      // clear/reset the set for each loop
        FD_SET(0, &readfds);    // add fd 0 to the set after clearing it each loop

        // call pselect()
        int p_return = pselect(1, &readfds, NULL, NULL, NULL, &zero);

        // check if pselect() was interrupted by a signal
        if (p_return == -1 && errno == EINTR) {
            // check if the signal was USR1 or USR2
            if (sigusr1_happened) {
                sigusr1_happened = 0; // reset the flag
                // if speed is already at 0, or somehow less than, set speed to 0
                if (speed <= 0) {
                    speed = 0;
                }
                // else decrement speed
                else {
                    speed--;
                }
                printf("decrease speed to: %d\n", speed);
            }
            else if (sigusr2_happened) {
                sigusr2_happened = 0;   // reset the flag
                speed++;                // increment the speed
                printf("increase speed to: %d\n", speed);
            }
        }
        
        // check if pselect() was interrupted by the user typing
        else if (p_return > 0 && FD_ISSET(0, &readfds)) {
            // if p_return is greater than 0, read what was written
            int bytes = read(0, buff, sizeof(buff));
            
            // loop through what was entered, check for '+', '-', and 'q', ignoring all other characters
            for (int i = 0; i < bytes; i++) {
                if (buff[i] == '+') {
                    speed++;
                    printf("increase speed to: %d\n", speed);
                }
                else if (buff[i] == '-') {
                    if (speed <= 0) {
                        speed = 0;
                    }
                    else {
                        speed--;
                    }
                    printf("decrease speed to: %d\n", speed);
                }
                else if (buff[i] == 'q') {
                    running = 0;    // break the loop
                }
            }
        }
    }
    return 0;
}

