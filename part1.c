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
#include <wait.h>

//could change to inputs later
//reminder to change populate also in that case
#define L 10000
#define H 60
#define PN 10

//find with DFS
void findHiddenKeysDFS(int keys[], int NUM_NEGATIVE_KEYS) {
//replace this loop numpber with PN amount you want to fork 
    pid_t pid[PN];
    int start; //start check for each process
    int end; //end check for each process
    int increment = L / PN; //so goes by 1000
    int foundKeys[NUM_NEGATIVE_KEYS]; //store found hidden keys 
    int foundKeysIndices[NUM_NEGATIVE_KEYS]; //store the place found for keys 
    int counter = 0; //to increment the above array
    for (int i = 0; i < PN; i++) {
        if((pid[i] = fork()) < 0) {
            perror("Failed to fork process");
            exit(EXIT_SUCCESS);
        }
            
        if (pid[i] > 0) {
            //leave loop
            start = increment * i;
            end = start + increment;
            break;
        } 
    }

    for (int i = start; i < end; i++) {
        if (keys[i] < 0) {
            foundKeysIndices[counter] = i + 1;
            foundKeys[counter] = keys[i];
            counter++;
        }
    }
    
    wait(NULL);

    if(counter > 1) {
        for (int i = 0; i < counter; i++) {
            printf("Hi I'm process %d with return arg ??? and my parent is %d. I found hidden key %d at position A[%d].\n", getpid(), getppid(), foundKeys[i], foundKeysIndices[i]);
        }
    }
    else {
        printf("Hi I'm process %d with return arg ??? and my parent is %d. I found hidden key %d at position A[%d].\n", getpid(), getppid(), foundKeys[counter], foundKeysIndices[counter]);
    }
    
}

void findKeyHelper() {

}

//find with BFS
void findHiddenKeysBFS(int keys[], int NUM_NEGATIVE_KEYS) {

}

int main(int argc, char const *argv[]) {
    //for future use
    if (argc != 3) {
        perror("Need arguments for L and H");
        exit(EXIT_FAILURE);
    }

    int keys[L];
    FILE *fp1 = fopen("integers.txt", "r");
    if (fp1 == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < L; i++) {
        fscanf(fp1, "%d", &keys[i]);
    }
    fclose(fp1);

    //find keys
    //findHiddenKeys();
    int pipe_fds[PN][2];
    for (int i = 0; i < PN; i++) {
        if (pipe(pipe_fds[i]) == -1) {
            perror("Error creating pipe");
            exit(EXIT_FAILURE);
        }
    }
    //findHiddenKeysDFS(keys, int start, int end, H);
    findHiddenKeysDFS(keys, H);
    //print resutls for each child
    //printf("Hi I'm process %d with return arg ??? and my parent is %d. I found hidden key %d at position A[%d].", getpid(), getppid(), something, something);


    exit(EXIT_SUCCESS);
}
