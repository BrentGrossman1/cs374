/*

Brent Grossman
02/08/2026
cs374
Project 5 - Append-Only Data Log

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define RECORD_COUNT 1048576
#define RECORD_SIZE 32
#define HEADER_SIZE (sizeof(int) * 2)
#define FILE_SIZE (HEADER_SIZE + (RECORD_COUNT * RECORD_SIZE))
#define LOG_FILE "log.dat"

// check for a valid number of arguments passed by the user at the CLI, must be odd and 3 or greater.
void valid_count(int argc, char *argv[]) {
    if (argc < 3 || argc % 2 == 0) {
        fprintf(stderr, "usage: %s [n string] [n string] ...\n", argv[0]);
        exit(1);
    }
}

// helper function to append records to the log file
void append_records(int fd, void *data, int count, char *prefix) {
    for (int i = 0; i != count; i++) {
        // lock settings
        struct flock lock = {
            .l_type = F_WRLCK,
            .l_whence = SEEK_SET,
            .l_start = 0,
            .l_len = HEADER_SIZE,
        };

        // header lock
        if ((fcntl(fd, F_SETLKW, &lock)) == -1) {
            perror("fcntl");
            exit(1);
        }
        // *header holds the mmap and points to index 0,
        int *header = (int *)data;
        int offset = header[0];     // saves current offset
        header[1]++;                // increment record count
        header[0] += RECORD_SIZE;   // update the offset in the header

        // header unlock
        lock.l_type = F_UNLCK;
        if ((fcntl(fd, F_SETLK, &lock)) == -1) {
            perror("fcntl");
            exit(1);
        }

        // update the lock settings to lock just the record being appended
        lock.l_start = HEADER_SIZE + offset;
        lock.l_len = RECORD_SIZE;
        lock.l_type = F_WRLCK;
        // record lock
        if ((fcntl(fd, F_SETLKW, &lock)) == -1) {
            perror("fcntl");
            exit(1);
        }

        // compute pointer to where record will be written
        char *record = (char *)data + HEADER_SIZE + offset;
        // create the sting in buffer
        char buffer[RECORD_SIZE];
        snprintf(buffer, RECORD_SIZE, "%s %d", prefix, i);
        // copy the buffer to the record
        strcpy(record, buffer);

        // unlock record
        lock.l_type = F_UNLCK;
        if ((fcntl(fd, F_SETLK, &lock)) == -1) {
            perror("fcntl");
            exit(1);
        }
    }
}

// helper function that locks the whole file to print all the records in the log
void dump_log(int fd, void *data) {
    struct flock lock = {
        .l_type = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start = 0,
        .l_len = 0,
    };
    // lock the file
    if ((fcntl(fd, F_SETLKW, &lock)) == -1) {
        perror("fcntl");
        exit(1);
    }

    // gets the count from the header
    int *header = (int *)data;
    int count = header[1];
    char *records = (char *)data + HEADER_SIZE;

    // loop to print each record
    for (int i = 0; i != count; i++) {
        char *curr_rec = records + (i * RECORD_SIZE);
        printf("%d: %s\n", i, curr_rec);
    }

    // unlock the file
    lock.l_type = F_UNLCK;
    if ((fcntl(fd, F_SETLK, &lock)) == -1) {
        perror("fcntl");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    valid_count(argc, argv);

    // initilize fd, then open the file, create if it doesnt exit, and truncate if it does
    int fd;
    if ((fd = open(LOG_FILE, O_RDWR | O_CREAT | O_TRUNC, 0666)) == -1) {
        perror("open");
        return 1;
    }

    ftruncate(fd, FILE_SIZE);

    // memory map to the file
    void *data = mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == NULL) {
        perror("mmap");
        return 2;
    }

    // loop and fork a child processes for each pair (count, prefix)
    for (int i = 1; i < argc; i += 2) {
        int count = atoi(argv[i]);
        char *prefix = argv[i + 1];

        pid_t PID = fork();

        if (PID == -1) {
            perror("fork");
            exit(1);
        }

        if (PID == 0) {
            // child process calls append function
            append_records(fd, data, count, prefix);
            exit(0);
        }
        // the parent will continue looping and creating child processes
    }

    // all child processes are running concurrently
    // parent waits for all to finish
    int num_pairs = (argc - 1) / 2;
    for (int i = 0; i < num_pairs; i++) {
        wait(NULL);
    }

    // print the log after all child processes finish appending
    dump_log(fd, data);

    // cleanup with munmap() and close()
    munmap(data, FILE_SIZE);
    close(fd);

    return 0;
}