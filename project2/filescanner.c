// Brent Grossman
// cs374 - Project 2: Scan Directories
// 01/18/2025

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
// Maximum path length for building full paths
#define PATH_SIZE 1024

/**
 * check_args - Validates command line arguments
 * argc: Number of command line arguments
 * 
 * Ensures no more than one directory argument is given.
 * Prints usage message and exits if validation fails.
 */
void check_args(int argc) {
    if(argc > 2) {
        fprintf(stderr, "usage: filescanner [directory]\n");
        exit(1);
    }
}

/**
 * scan_dir - Recursively scans a directory and calculates total size
 * dirpath: Path to the directory to scan
 * 
 * Returns: Total size in bytes of all regular files in the directory
 *          and its subdirectories
 * 
 * This function:
 * - Opens and reads the specified directory
 * - Skips symbolic links and special directories (. and ..)
 * - Adds sizes of regular files to the total
 * - Recursively scans subdirectories
 * - Prints results in post-order (subdirectories before parent)
 */
long scan_dir(char *dirpath) {
    DIR *d = opendir(dirpath);
    struct dirent *currdir;
    struct stat buff;
    long currdir_size = 0;

    // check if directory opened successfully
    if(d == NULL) {
        perror("opendir");
        exit(1);
    }

    // loop through each entry in the directory
    while((currdir = readdir(d)) != NULL) {
        char *filename = currdir->d_name;
        char fullpath[PATH_SIZE];
        // build the full path by combining directory path and filename
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dirpath, filename);

        // skip current dir and parent dir to avoid infinite recursion
        if(strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) {
            continue;
        }

        // get file information using lstat
        if(lstat(fullpath, &buff) == -1) {
            perror("lstat");
            continue;
        }
        // if its a symlink, skip past it
        if(S_ISLNK(buff.st_mode)) {
            continue;
        }
        // if its a regular file, add the file size to currdir_size total
        else if(S_ISREG(buff.st_mode)) {
            currdir_size += buff.st_size;
        }
        // if its a dir, recursively scan the subdir and add their total to current total
        else if(S_ISDIR(buff.st_mode)) {
            long subdir_size = scan_dir(fullpath);
            currdir_size += subdir_size;
        }
    }

    // print the dir total size post-order after all contents are totalled up
    printf("%s: %ld\n", dirpath, currdir_size);
    closedir(d);

    return currdir_size;
}

/**
 * main - Entry point for the file scanner program
 * argc: Number of command line arguments
 * argv: Array of command line argument strings
 * 
 * Returns: 0 on success
 * 
 * Validates arguments and initiates directory scan.
 * Uses current directory if no argument provided.
 */
int main(int argc, char *argv[]) {
    check_args(argc);

    if(argc == 1) {
        scan_dir(".");
    }
    else {
        scan_dir(argv[1]);
    }

    return 0;

}
