#include <stdio.h>

int main() {
    int x = 12;
    float y = 3.14;
    char* s = "Hello, world!";

    printf("x is %d, y is %g\n, %s\n", x, y, s);

    int i;
    int j = 5;
    int k;
    
    for(i = 0; i <5; i++) {
        k = i * j;
        printf("%d x %d = %d\n", i, j, k);
    }

    return 0;
}