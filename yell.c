#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    char buffer[2048];
    int bytes_read;
    int fd;

    // no files specified
    if(argc == 1) {
        fd = 0; // stdin file descriptor 0

        // keep reading until the end of the file
        while((bytes_read = read(fd, buffer, 2048)) > 0) {
            for(int j = 0; j < bytes_read; j++) {
                buffer[j] = toupper(buffer[j]);
            }
            write(1, buffer, bytes_read);
        }
    }

    // if files are specified
    else {
        for(int i = 1; i < argc; i++) {
            fd = open(argv[i], O_RDONLY);
            if(fd == -1) {
                perror("open");
                continue;
            }

            // keep reading until the end of the file
            while((bytes_read = read(fd, buffer, 2048)) > 0) {
                for(int j = 0; j < bytes_read; j++) {
                    buffer[j] = toupper(buffer[j]);
                }
                write(1, buffer, bytes_read);
            }

            close(fd);
        }   
    }

    return 0;
}