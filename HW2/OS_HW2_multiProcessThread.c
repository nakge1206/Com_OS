#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/wait.h>

#define NUM_THREADS 4
#define NUM_PROCESS 4
#define NUM_VALUES 100
#define THREAD_VALUES (NUM_VALUES / NUM_THREADS)

typedef struct {
    int process_value;
    int start_index;
    int process_num;
    int thread_num;
} ThreadArgs;

void *multiply(void *args) {
    ThreadArgs *thread_args = (ThreadArgs *)args;
    int process_value = thread_args->process_value;
    int start_index = thread_args->start_index;
    int process_num = thread_args->process_num;
    int thread_num = thread_args->thread_num;

    printf("%d 프로세스, %d 쓰레드 : \n", process_num, thread_num);
    // Perform multiplication and print
    for (int i = start_index; i < start_index + THREAD_VALUES; i++) {
        printf("%d ", process_value * (i + 1));
    }
    printf("\n");
    pthread_exit(NULL);
}

int main() {
    // Process and thread variables
    pid_t pid[NUM_PROCESS];
    pthread_t threads[NUM_THREADS];
    int process_values[NUM_PROCESS] = {3, 5, 7, 9};

    // Time measurement variables
    struct timeval start, end;

    // Start time measurement
    gettimeofday(&start, NULL);

    // Create processes
    for (int i = 0; i < NUM_PROCESS; i++) {
        pid[i] = fork();
        if (pid[i] == 0) { // Child process
            printf("\nProcess %d started.\n", i + 1);
            for (int k = 0; k < i; k++) {
                waitpid(pid[k], NULL, 0);
            }
            // Create threads   
            for (int j = 0; j < NUM_THREADS; j++) {
                ThreadArgs *args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
                args->process_value = process_values[i];
                args->start_index = j * THREAD_VALUES;
                args->process_num = i + 1; // Initialize process_num
                args->thread_num = j + 1; // Initialize thread_num
                pthread_create(&threads[j], NULL, multiply, (void *)args);
            }
            printf("Process %d finished.\n", i + 1);
            exit(0);
        }
    }

    // Wait for all processes to finish
    for (int i = 0; i < NUM_PROCESS; i++) {
        waitpid(pid[i], NULL, 0);
    }

    // End time measurement
    gettimeofday(&end, NULL);

    // Calculate and print execution time
    double execution_time = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Total execution time: %f seconds.\n \n", execution_time);


    //싱글프로세스-싱글쓰레드 환경
    gettimeofday(&start, NULL);
    printf("3곱하기 : \n");
    for(int i = 1; i <= 100; i++){
        if(i == 26 || i == 51 || i == 76){
            printf("\n");
        }
        printf("%d ", i * 3);
    }
    printf("\n \n");

    printf("5곱하기 : \n");
    for(int i = 1; i <= 100; i++){
        if(i == 26 || i == 51 || i == 76){
            printf("\n");
        }
        printf("%d ", i * 5);
    }
    printf("\n \n");

    printf("7곱하기 : \n");
    for(int i = 1; i <= 100; i++){
        if(i == 26 || i == 51 || i == 76){
            printf("\n");
        }
        printf("%d ", i * 7);
    }
    printf("\n \n");

    printf("9곱하기 : \n");
    for(int i = 1; i <= 100; i++){
        if(i == 26 || i == 51 || i == 76){
            printf("\n");
        }
        printf("%d ", i * 9);
    }
    printf("\n \n");

    // End time measurement
    gettimeofday(&end, NULL);

    // Calculate and print execution time
    double execution_time_single = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Total execution time: %f seconds.\n", execution_time_single);
    return 0;
}
