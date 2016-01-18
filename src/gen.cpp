#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    srand(time(NULL));
    int n = atoi(argv[1]);
    printf("%d\n", n);
    while (n--) {
        printf("%d ", rand());
    }
    printf("\n");
    return 0;
}
