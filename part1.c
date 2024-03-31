#include <stddef.h>
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

//could change to inputs later
//reminder to change populate also in that case
#define NUM_KEYS 10000
#define NUM_NEGATIVE_KEYS 60

//find with DFS
void findHiddenKeys(int keys[], int start, int end, int H) {

}

int main(int argc, char const *argv[]) {
    //for future use
    if (argc != 3) {
        perror("Need arguments for L and H");
        exit(EXIT_FAILURE);
    }

    int keys[NUM_KEYS];
    FILE *fp1 = fopen("integers.txt", "r");
    if (fp1 == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < NUM_KEYS; i++) {
        fscanf(fp1, "%d", &keys[i]);
    }
    fclose(fp1);

    //find keys
    //findHiddenKeys();

    exit(EXIT_SUCCESS);
}
