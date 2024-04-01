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
#define L 10000
#define H 60
#define PN 10

//find with DFS
void findHiddenKeysDFS(int keys[], int start, int end, int NUM_NEGATIVE_KEYS) {
//replace this loop numpber with PN amount you want to fork 
    pid_t pid[PN];
    for (int i = 0; i < PN; i++) {
        if((pid[i] = fork()) < 0) {
            perror("Failed to fork process");
            exit(EXIT_SUCCESS);
        }
            
        if (pid[i] > 0) {
            //leave loop
            break;
        } 
    }
}

void findKeyHelper() {

}

//find with BFS
void findHiddenKeysBFS(int keys[], int start, int end, int NUM_NEGATIVE_KEYS) {

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

    exit(EXIT_SUCCESS);
}
