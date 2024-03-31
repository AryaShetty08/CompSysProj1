#include <strings.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <time.h>

void printRandoms(int lower, int upper, int count) {
    for (int i = 0; i < count; i++) {
        int num = (rand() %
        (upper - lower + 1)) + lower;
        printf("%d ", num);
    }
}

int main(int argc, char const *argv[]) {
    FILE *fp1;
    fp1 = fopen("integers.txt", "a+");

    srand(time(NULL));
    int r = rand();

    printRandoms(-60, -1, 10);

    fclose(fp1);

    exit(EXIT_SUCCESS);
}
