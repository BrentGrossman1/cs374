#include <stdio.h>

int main(int argc, char *argv[]) {
    // Use a for loop to go from 0 to argc-1
    // For each iteration, print the index and the argument
    int i;
    for(i = 0; i < argc; i++) {
        printf("%d: %s\n", i, argv[i]);
    }
    
    return 0;
}