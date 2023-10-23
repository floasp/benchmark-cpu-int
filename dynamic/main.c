#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "utility.h"
#include "benchmark.h"


pthread_mutex_t mutex_next_nr;

int main(){
    printf("Starting Benchmark!\n");

    mutex_next_nr = PTHREAD_MUTEX_INITIALIZER;
    timings_t timing_values;
    int cores = getNumberOfCores();
    printf("Number of logical CPU cores: %d\n", cores);

    int n_tests = 7;
    //int test_n[7] = {1280, 2560, 5120, 10240, 20480, 40960, 81920};
    //int test_n[7] = {1000, 2000, 5000, 10000, 20000, 50000, 100000};
    int test_n_start[7] = {0, 0, 0, 0, 3, 2001, 2000};  // starting point for each benchmark run
    int test_n_end[7] = {1000, 2000, 2000, 2000, 2000, 2000, 5000};  // end point for each benchmark run
    double results[7] = {0};

    printf("Starting Calculations...\n");

    for(int i = 0; i < n_tests; i++){
        results[i] = calculate(&test_n_start[i], &test_n_end[i], cores, &timing_values, &mutex_next_nr);
        if(results[i] > 200){
            break;
        }
        printf("Cooldown...\n");

        my_sleep(1);
    }

    FILE *file;
    file = fopen("results.dat", "w+");
    if(file){
        fprintf(file, "cores=%d \n", cores);
    }

    printf("Results:\n");
    if(file){
        fprintf(file, "Results:\n");
    }

    for(int i = 0; i < n_tests; i++){
        if(!results[i]){
            break;
        }
        printf("(%d -> %d): %lf\n", test_n_start[i], test_n_end[i], results[i]);
        if(file){
            fprintf(file, "(%d -> %d): %lf\n", test_n_start[i], test_n_end[i], results[i]);
        }
    }

    fclose(file);

    return 0;
}