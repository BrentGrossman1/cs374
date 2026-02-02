/*

Brent Grossman
02/01/2026
cs374
Project 4 - Use Pipes for Interprocess Communication:
            Part 2 - redirout

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {

    int fd;
    
    // Check if user entered enough arguments
    if (argc < 3) {
        fprintf(stderr, "usage: redirout filename command [arg1 [arg2 ... ] ]\n");
        exit(1);
    }

    // open the file
    fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if(fd == -1) {
        perror("open");
        exit(1);
    }

    // dup2 to redirect stdout
    dup2(fd, 1);
    close(fd);

    // execvp the command
    execvp(argv[2], argv + 2);
    perror("execvp");
    exit(1);
    
    return 0;
}