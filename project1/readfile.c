#include <fcntl.h>    // for open()
#include <unistd.h>   // for read(), write(), close()
#include <stdio.h>    // for perror() 

int main(int argc, char *argv[]) {
    // gets the file name from argv[1]
    char *filename = argv[1];

    // open the file
    int fd = open(filename, O_RDONLY); // O_RDONLY means read only
    if(fd == -1) {
        perror("open");
        return 1;
    }

    // read from the file up to 2048bytes
    char buffer[2048];
    int bytes_read = read(fd, buffer, 2048);

    // write to stdout file descriptor 1
    write(1, buffer, bytes_read); 

    // close the file
    close(fd);

    return 0;
}