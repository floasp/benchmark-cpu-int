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

int main(int argc, char *argv[]){
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

    printf("Starting Calculations...\n");

    benchmark_results_t results;
    benchmark_setup_t setup;
    setup.n_tests = n_tests;
    setup.test_n_start = test_n_start;
    setup.test_n_end = test_n_end;
    setup.cores = cores;
    setup.max_prev_time = 200;  // next run will only be started when previous run was less than this in seconds
    setup.cooldown_s = 1; // cooldown between successive runs in seconds
    setup.timing_values = &timing_values;
    setup.mutex_next_nr = &mutex_next_nr;

    int error = benchmark_run(&results, &setup);
    
    if(error){
        printf("Error during benchmark...");
        return 0;
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
        if(!results.results[i]){
            break;
        }
        printf("(%d -> %d): %lf\n", test_n_start[i], test_n_end[i], results.results[i]);
        if(file){
            fprintf(file, "(%d -> %d): %lf\n", test_n_start[i], test_n_end[i], results.results[i]);
        }
    }

    fclose(file);

    return 0;
}