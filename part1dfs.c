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

int main(int argc, char const *argv[]) {
    double timeSpent = 0.0;
    clock_t begin = clock();
    
    //make sure you have all arguments
    if (argc != 4) {
        perror("Need arguments for L, H, and PN");
        exit(EXIT_FAILURE);
    }

    int L = atoi(argv[1]);
    int H = atoi(argv[2]);
    int PN = atoi(argv[3]);


    if (L < 10000) {
        printf("L must be greater than or equal to 10000\n");
        exit(EXIT_FAILURE);
    }

    if (H < 30 || H > 60) {
        printf("H must be greater than or equal to 30 and less than or equal to 60\n");
        exit(EXIT_FAILURE);
    }

    if (PN < 1) {
        printf("PN must be greater than or equal to 1\n");
        exit(EXIT_FAILURE); 
    }

    //set file pointers and array of keys to search 
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
    
    FILE* output;
    output = fopen("output.txt", "a+");

    if (output == NULL) {
        perror("File could not be opened");
        exit(EXIT_FAILURE);
    }

    int pipe_fds[PN][2];
    int pipe_bds[PN][2];

    int pid;
    int start = 0;
    int end = 0;

    int parentRoot = getpid();  //Head of tree
    int returnArg = 1; 

    //create pipes for each child 
    for (int i = 0; i < PN; i++) {
        if (pipe(pipe_fds[i]) == -1) {
            perror("Error creating pipe");
            exit(EXIT_FAILURE);
        }

        if (pipe(pipe_bds[i]) == -1) {
            perror("Error creating pipe");
            exit(EXIT_FAILURE);
        }
    }

    //Use idea of dfs and fork accordingly 
    for (int i = 0; i < PN; i++) {
        pid = fork();

        if (pid == - 1) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) {
           //show each child process created 
            printf("Hi I'm process %d with return arg %d and my parent is %d.\n", getpid(), returnArg + 1, getppid());
            fprintf(output, "Hi I'm process %d with return arg %d and my parent is %d.\n", getpid(), returnArg + 1, getppid());

            returnArg++;
            start = end; //since you are on next process
            end = end + L / PN; //to get the next increment 
            if (end > L) { //make sure it doesn't go over the amount of integers 
                end = L; //Set to max integers in keys 
            }
            if (end == L-1) {
                end ++;
            }

                int max = 0;
                double average = 0;
                int count = 0;

                //calculate the local max and average 
                for (int j = start; j < end; j++) {
                    if (keys[j] > max) {
                        max = keys[j];
                    }
                    average += keys[j];
                    count++;
                }

                average = average/ (double) count;

                //write local max, average, count to corresponding child pipe 
                //also get the amount of hidden keys left to find
                write(pipe_fds[i][1], &max, sizeof(int)); 
                write(pipe_fds[i][1], &average, sizeof(double));
                write(pipe_fds[i][1], &count, sizeof(count));
                read(pipe_bds[i][0], &H, sizeof(int));

                //search for keys in given child interval 
                for (int i = start; i < end; i++) {
                    if (H != 0 && keys[i] < 0) {
                        printf("Hi I am Process %d with return arg %d. I found the hidden key in position A[%d].\n", getpid(), returnArg, i+1); 
                        fprintf(output, "Hi I am Process %d with return arg %d. I found the hidden key in position A[%d].\n", getpid(), returnArg, i+1);
                        H--;
                    }
                }
                //let next child know how many keys left to find
                write(pipe_bds[i+1][1], &H, sizeof(int));
        }
        else { //we are in parent process
            int tempMax;
            double tempAverage; 
            int tempCount;
            int max = 0;
            double average = 0;
            int count = 0;

            //check if start of chain 
            if (parentRoot != getpid()) {
                //just wait for child terminate before exiting
                wait(NULL);
                exit(EXIT_SUCCESS);
            }
            else { //if it is parentroot
                //write the hidden keys to find for child 
                write(pipe_bds[i][1], &H, sizeof(int));

                wait(NULL); //wait for first child to finish 
                
                //Check all the children pipes made to find the total max, average, and count of the integers array 
                for (int k = 0; k < PN; k++) {
                    read(pipe_fds[k][0], &tempMax, sizeof(int));
                    read(pipe_fds[k][0], &tempAverage, sizeof(double));
                    read(pipe_fds[k][0], &tempCount, sizeof(int));
                    
                    if (tempMax >= max) {
                        max = tempMax;
                    }
                    
                    if (tempAverage > 0) {
                        average = (average * count + tempAverage * tempCount) / (double)(tempCount + count);
                    }

                    count += tempCount;
                }
 
                //Print out all required statements and close files 
                printf("Max: %d, Average: %f\n\n", max, average);
                fprintf(output, "Max: %d, Average: %f\n\n", max, average);
                clock_t end = clock();
                timeSpent += (double)(end - begin) / CLOCKS_PER_SEC;
                printf("\nThe program completed in %f seconds \n", timeSpent);
                fprintf(output, "\nThe program completed in %f seconds \n", timeSpent);
                fclose(output);
                exit(EXIT_SUCCESS);
            }
        }
    }    
    exit(EXIT_SUCCESS);
}
