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
#include <stdbool.h>

// initialising global variables
int L, H, PN;

int main(int argc, char const *argv[]) {
    if (argc != 4) {
        perror("Invalid number of arguments");
        exit(EXIT_FAILURE);
    }

    L = atoi(argv[1]);
    H = atoi(argv[2]);
    PN = atoi(argv[3]);

    // check constraints for inputs
    if (L < 10000 || L > 1000000) {
        perror("Invalid value for L");
        exit(EXIT_FAILURE);
    }
    if (H < 30 || H > 60) {
        perror("Invalid value for H");
        exit(EXIT_FAILURE);
    }
    if (PN < 1) {
        perror("Invalid value for PN");
        exit(EXIT_FAILURE);
    }

    // ask the user how many children each fork should create
    int maxChildren;
    printf("How many children should each fork create? (2,3) \n");
    scanf("%d", &maxChildren);

    // checks constraints on maxChildren
    if (maxChildren != 2 && maxChildren != 3) {
        perror("Invalid value for maxChildren");
        exit(EXIT_FAILURE);
        return -1;
    }

    // some initializations
    // pid_t pid[PN];
    int rootNode = getpid();
    int keys[L];
    // int foundKeysIndices[L];
    // int foundKeys[L];
    int start = 0, end = L, oldEnd;
    // time variables
    clock_t startTime = clock(), endTime;
    double timeSpent = 0.0;

    // reads the file and stores the values in the keys array
    FILE *fp = fopen("integers.txt", "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    } else {
        for (int i = 0; i < L; i++) {
            fscanf(fp, "%d", &keys[i]);
        }
    }
    fclose(fp);

    // initalize array with pid of children who found the key, their return argument code, and the index of the key
    int returnH[H*3];
    int startH = 0;
    // initialize counter for printing
    int returnArg = 1;
    int returnCode;
    int fd[2*PN];
    // tracks the parent pipe
    int parentPipe;
    // tracks the number of children in BFS
    int bfsChildren[3] = {-1, -1, -1};
    int pid;
    pid_t childTracker;
    int childCounter = -1;

    // loops through and creates PN number of children
    for (int i = 0; i < floor(log2(PN)); i++) {
        // does not update on the first iteration
        if (i != 0) {
            // updates returnArg based on maxChildren
            if (maxChildren == 2) {
                returnArg = returnArg * 2 - 1;
            } else {
                returnArg = returnArg * 3 - 2;
            }
        }

        childTracker = getpid();
        int increment = ceil((end-start) / maxChildren);
        oldEnd = end;
        end = start;

        for (int j = 0; j < maxChildren; j++) {
            if (childTracker == getpid() && returnArg < PN) {
                for (int k = 0; k < maxChildren; k++) {
                    if (bfsChildren[k] == -1) {
                        bfsChildren[k] = returnArg - 1;
                        break;
                    }
                }

                pipe(&fd[2*(returnArg-1)]);
                pid = fork();

                if(j != 0) {
                    start = start + increment;
                }
                end = end + increment;

                if (L - end < 5) {
                    end = L;
                }
                returnArg += 1;
                returnCode = returnArg;
            }
        }

        if (pid == -1) {
            perror("Failed to fork process");
            // exit(EXIT_FAILURE);
        } else if (pid == 0) {          // child process
            for (int j = 0; j < maxChildren; j++) {
                if (bfsChildren[j] != -1) {
                    parentPipe = bfsChildren[j];
                    bfsChildren[j] = -1;
                }
            }

            printf("Hi I'm process %d with return arg %d and my parent is %d.\n", getpid(), returnArg, getppid());

            pid = getpid();
            if (i == floor(log2(PN)) - 1) {
                int max = 0, count = 0;
                int64_t avg = 0;
                for (int j = start; j < end; j++) {
                    if (keys[j] > max) {
                        max = keys[j];
                    }
                    avg += keys[j];
                    if (keys[j] < 0) {
                        returnH[startH] = getpid();
                        returnH[startH+1] = j;
                        returnH[startH+2] = returnCode;
                        startH += 3;
                    }
                }

                avg = avg / (end - start);
                count = end - start;
                close(fd[2*parentPipe]);
                write(fd[2*parentPipe+1], &max, sizeof(max));
                write(fd[2*parentPipe+1], &avg, sizeof(avg));
                write(fd[2*parentPipe+1], &count, sizeof(count));
                write(fd[2*parentPipe+1], &returnH, sizeof(int) * (H*3));
                write(fd[2*parentPipe+1], &startH, sizeof(startH));
                close(fd[2*parentPipe+1]);

                exit(0);
            }
        } else {
            // parent process

            bool hasChild = false, tracked = false;
            int64_t avg = 0;
            int childCount = 0, max = 0, count = 0, tempStartH = 0, tempMax = -1, tempAvg = -1, tempCount = -1;
            int tempReturnH[H*3];
            
            for (int j = 0; j < maxChildren; j++) {
                if (bfsChildren[j] != -1) {
                    hasChild = true;
                    childCount++;
                } else if (bfsChildren[j] == -1 && hasChild && !tracked) {
                    start = end;
                    end = oldEnd;
                    tracked = true;
                }
            }

            if (!hasChild) {
                end = oldEnd;
            }

            if (childCount == 0) {
                for (int j = start; j < end; j++) {
                    if (keys[j] > max) {
                        max = keys[j];
                    }
                    avg += keys[j];
                    if (keys[j] < 0) {
                        returnH[startH] = getpid();
                        returnH[startH+1] = j;
                        returnH[startH+2] = returnCode;
                        startH += 3;
                    }
                }
                avg = avg / (end - start);
                count = end - start;
            } else if (childCount < maxChildren) {
                for (int j = start; j < end; j++) {
                    if (keys[j] > max) {
                        max = keys[j];
                    }
                    avg += keys[j];
                    if (keys[j] < 0) {
                        returnH[startH] = getpid();
                        returnH[startH+1] = j;
                        returnH[startH+2] = returnCode;
                        startH += 3;
                    }
                }
                avg = avg / (end - start);
                count = end - start;

                for (int j = 0; j < childCount; j++) {
                    read(fd[2*bfsChildren[j]], &tempMax, sizeof(tempMax));
                    read(fd[2*bfsChildren[j]], &tempAvg, sizeof(tempAvg));
                    read(fd[2*bfsChildren[j]], &tempCount, sizeof(tempCount));
                    read(fd[2*bfsChildren[j]], &tempReturnH, sizeof(tempReturnH));
                    read(fd[2*bfsChildren[j]], &tempStartH, sizeof(tempStartH));

                    if (tempMax >= max) {
                        max = tempMax;
                    }

                    avg = (avg * count + tempAvg * tempCount) / (tempCount + count);
                    count += tempCount;

                    for (int k = 0; k < tempStartH; k++) {
                        returnH[startH] = tempReturnH[k];
                        startH++;
                    }
                }
            } else {
                for (int j = 0; j < childCount; j++) {
                    if (j != 0) {
                        read(fd[2*bfsChildren[j]], &tempMax, sizeof(tempMax));
                        read(fd[2*bfsChildren[j]], &tempAvg, sizeof(tempAvg));
                        read(fd[2*bfsChildren[j]], &tempCount, sizeof(tempCount));
                        read(fd[2*bfsChildren[j]], &tempReturnH, sizeof(tempReturnH));
                        read(fd[2*bfsChildren[j]], &tempStartH, sizeof(tempStartH));

                        if (tempMax >= max) {
                            max = tempMax;
                        }

                        avg = (avg * count + tempAvg * tempCount) / (tempCount + count);
                        count += tempCount;

                        for (int k = 0; k < tempStartH; k++) {
                            returnH[startH] = tempReturnH[k];
                            startH++;
                        }
                    } else {
                        read(fd[2*bfsChildren[j]], &max, sizeof(max));
                        read(fd[2*bfsChildren[j]], &avg, sizeof(avg));
                        read(fd[2*bfsChildren[j]], &count, sizeof(count));
                        read(fd[2*bfsChildren[j]], &returnH, sizeof(int) * (H*3));
                        read(fd[2*bfsChildren[j]], &startH, sizeof(startH));
                    }
                }
            }

            if (rootNode != getpid()) {
                close(fd[2*PN]);
                write(fd[2*PN+1], &max, sizeof(max));
                write(fd[2*PN+1], &avg, sizeof(avg));
                write(fd[2*PN+1], &count, sizeof(count));
                write(fd[2*PN+1], &returnH, sizeof(int) * (H*3));
                write(fd[2*PN+1], &startH, sizeof(startH));
                close(fd[2*PN+1]);
            } else {
                printf("brug");
                wait(NULL);
                printf("Max: %d, Average: %f\n", max, (double) avg);

                for (int j = 0; j < H*3; j+=3) {
                    printf("Hi I am Process %d with return arg %d. I found the hidden key in position A[%d].\n", returnH[j], returnH[j+2], returnH[j+1]);
                }

                endTime = clock();
                timeSpent = (double) (endTime - startTime) / CLOCKS_PER_SEC;
                printf("\nThe program completed in %f seconds \n", timeSpent);
                exit(EXIT_SUCCESS);
            }

            wait(NULL);
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}
