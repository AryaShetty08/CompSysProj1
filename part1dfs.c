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
    //for future use
    double timeSpent = 0.0;
    clock_t begin = clock();
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

    int pipe_fds[PN][2];
    int pipe_bds[PN][2];

    int pid;
    int start = 0;
    int end = 0;

    int parentRoot = getpid();  //Head of tree
    int returnArg = 1; 

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

    for (int i = 0; i < PN; i++) {
        pid = fork();

        if (pid == - 1) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) {
           
            printf("Hi I'm process %d with return arg %d and my parent is %d.\n", getpid(), returnArg + 1, getppid());

            returnArg++;
            start = end; //since you are on next process
            end = end + L / PN; //to get the next increment 
            if (end > L) { //make sure it doesn't go over the amount of integers 
                end = L; //Set to max integers in keys 
            }

            //check if the child is the last in PN
            //if (i == (PN - 1)) {
                //close(pipe_fds[i][0]); //close read end 
                //close(pipe_bds[i+1][0]); 

                int max = 0;
                double average = 0;
                int count = 0;

                for (int j = start; j < end; j++) {
                    //if (keys[j] < 0) {
                    //    continue;
                    //}

                    if (keys[j] > max) {
                        max = keys[j];
                    }

                    average += keys[j];
                    count++;
                }
                printf("COUNT in CHILD %d\n", count);
                average = average/ (double) count;
                write(pipe_fds[i+1][1], &max, sizeof(int)); //test
                write(pipe_fds[i+1][1], &average, sizeof(double));
                write(pipe_fds[i+1][1], &count, sizeof(count));
                //close(pipe_fds[i+1][1]);
                
                //update how many H left to find
                read(pipe_bds[i][0], &H, sizeof(int));
                //close(pipe_bds[i][0]);

                for (int i = start; i < end; i++) {
                    if (H != 0 && keys[i] < 0) {
                        printf("Hi I am Process %d with return arg %d. I found the hidden key in position A[%d].\n", getpid(), returnArg, i+1); 
                        H--;
                    }
                }
                write(pipe_bds[i+1][1], &H, sizeof(int));
                //exit(EXIT_SUCCESS);
            //}
          
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
                //closes hella pipes
               // count = 0;
               // for (int j = start; j < end; j++) {
                 //   if (keys[j] < 0) {
                   //     continue; 
                   // }

                   // if (keys[j] > max) {
                   //     max = keys[j];
                   // }
                   // average += keys[j];
                   // count++;
               // }

                //average = average / (double) count;

                read(pipe_fds[i-1][1], &max, sizeof(int));
                read(pipe_fds[i-1][1], &average, sizeof(double));
                read(pipe_fds[i-1][1], &count, sizeof(int));

                printf("COUNT at %d: %d\n", i-1, count);
                //write(pipe_fds[i-1][1], &max, sizeof(int));
                //write(pipe_fds[i-1][1], &average, sizeof(double));
                //write(pipe_fds[i-1][1], &count, sizeof(int));

                read(pipe_fds[i][0], &tempMax, sizeof(int));
                read(pipe_fds[i][0], &tempAverage, sizeof(double));
                read(pipe_fds[i][0], &tempCount, sizeof(int));
                //close(pipe_fds[i][0]);
                printf("TEMPCOUNT at %d: %d\n", i, tempCount);

                if (tempMax >= max) {
                    max = tempMax;
                }

                average = (average * count + tempAverage * tempCount) / (double)(tempCount + count);

                double newCount = (double)tempCount + count;
                printf("NEWCOUNT: %f\n",newCount);

                write(pipe_fds[i][1], &max, sizeof(int));
                write(pipe_fds[i][1], &average, sizeof(double));
                write(pipe_fds[i][1], &newCount, sizeof(int));
                //close(pipe_fds[i+1][1]);

                //read(pipe_bds[i-1][0], &H, sizeof(int));
                //close(pipe_bds[i][0]);

                //for (int i = start; i < end; i++) {
                  //  if(H != 0 && keys[i] < 0) {
                        //printf("Hi I am Process %d with return arg %d. I found the hidden key in position A[%d].\n", getpid(), returnArg, i); 
                    //    H--;
                   // }
                //}
                //write(pipe_bds[i][1], &H, sizeof(int));
                //close(pipe_bds[i+1][1]);
                wait(NULL);
                exit(EXIT_SUCCESS);
            }
            else { //if it is parentroot
                //close pipes here 
                write(pipe_bds[i][1], &H, sizeof(int));
                write(pipe_fds[i][1], &max, sizeof(int)); //test
                write(pipe_fds[i][1], &average, sizeof(double));
                write(pipe_fds[i][1], &count, sizeof(count));

                wait(NULL); //wait for first child to finish 
                read(pipe_fds[PN-1][0], &max, sizeof(int)); 
                read(pipe_fds[PN-1][0], &average, sizeof(double));
                read(pipe_fds[PN-1][0], &count, sizeof(int));

                int max2 = 0;
                double average2 = 0;
                int count2 = 0;

                read(pipe_fds[PN-2][0], &max2, sizeof(int)); 
                read(pipe_fds[PN-2][0], &average2, sizeof(double));
                read(pipe_fds[PN-2][0], &count2, sizeof(int));

                if (max2 >= max) {
                    max = max2;
                }

                printf("%d, %d\n",  count, count2);
                double averageF = (average * count + average2 * count2) / (double)(count2 + count);

                printf("Max: %d, Average: %f\n\n", max, averageF);
                //close
                clock_t end = clock();
                timeSpent += (double)(end - begin) / CLOCKS_PER_SEC;
                printf("\nThe program completed in %f seconds \n", timeSpent);
                exit(EXIT_SUCCESS);

            }
        }
    }    
    //printf("Hi I'm process %d with return arg ??? and my parent is %d. I found hidden key %d at position A[%d].", getpid(), getppid(), something, something);
    exit(EXIT_SUCCESS);
}
