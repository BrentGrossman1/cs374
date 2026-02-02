/*

Brent Grossman
02/01/2026
cs374
Project 4 - Use Pipes for Interprocess Communication:
            Part 1 - lswc

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {
    char *dir;
    int pipefd[2];

    // parse command line
    if(argc == 1) {
        dir = ".";
    }
    else if(argc >= 2) {
        dir = argv[1];
    }

    // create the pipe
    if(pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    int write_fd = pipefd[1];
    int read_fd = pipefd[0];

    // fork
    if(!fork()) {
        // the child runs wc (reads from the pipe)
        close(write_fd);
        dup2(read_fd, 0);
        close(read_fd);
        execlp("wc", "wc", "-l", NULL);
        perror("wc");
        exit(1);
    }
    else {
        // the parent runs ls (writes to the pipe)
        close(read_fd);
        dup2(write_fd, 1);
        close(write_fd);
        execlp("ls", "ls", "-1a", dir, NULL);
        perror("ls");
        exit(1);
    }
    return 0;
}