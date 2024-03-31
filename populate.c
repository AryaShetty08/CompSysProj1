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

#define SIZE 10000
#define NUM_NEGATIVE_KEYS 60

//Generate random positive integers
int generatePositiveInt() {
    return rand() % 1000 + 1; // change number to decide what range
}

//Generate random negative integers
int generateNegativeInt() {
    return -1 * (rand() % NUM_NEGATIVE_KEYS + 1); //based on H generate keys  
}

//randomize integer order
void shuffleNumbers(int *array, int n) {
    if (n > 1) {
        for (int i = 0; i < n-1; i++) {
            int j = i + rand() / (RAND_MAX / (n-i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

//Input integers into text file
void writeToFile(int numbers[], int size) {
    FILE *fp1 = fopen("integers.txt", "w+");
    if (fp1 == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < size; i++) {
        fprintf(fp1, "%d\n", numbers[i]);
    }

    fclose(fp1);
}

//Calculate max of numbers
int calculateMax(int numbers[], int size) {
    int max = numbers[0];
    for (int i = 1; i < size; i++) {
        if (numbers[i] > max) {
            max = numbers[i];
        }
    }
    return max;
}

//Calculate average
float calculateAverage(int numbers[], int size) {
    float sum = 0;
    for (int i = 0; i < size; i++) {
        sum += numbers[i];
    }
    return sum/size;
}

//main function
int main(int argc, char const *argv[]) {
    int numbers[SIZE];
    srand(time(NULL)); //seed

    for (int i = 0; i < SIZE - NUM_NEGATIVE_KEYS; i++) {
        numbers[i] = generatePositiveInt();
    }

    for (int i = SIZE - NUM_NEGATIVE_KEYS; i < SIZE; i++) {
        numbers[i] = generateNegativeInt();
    }

    shuffleNumbers(numbers, SIZE);
    writeToFile(numbers, SIZE);

    printf("MAX: %d\n", calculateMax(numbers, SIZE));
    printf("AVG: %.2f\n", calculateAverage(numbers, SIZE));

    exit(EXIT_SUCCESS);
}
