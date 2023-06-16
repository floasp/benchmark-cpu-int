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

    //setup_mutex(&mutex_next_nr);
    mutex_next_nr = PTHREAD_MUTEX_INITIALIZER;
    timings_t timing_values;
    int cores = getNumberOfCores();
    printf("Number of logical CPU cores: %d\n", cores);

    int n_tests = 7;
    //int test_n[7] = {1280, 2560, 5120, 10240, 20480, 40960, 81920};
    //int test_n[7] = {1000, 2000, 5000, 10000, 20000, 50000, 100000};
    int test_n[7] = {1000, 2000, 2000, 2000, 2000, 2000, 5000};
    double sc_results[7] = {0};
    double mc_results[7] = {0};

    calculate(100, 1, &timing_values, &mutex_next_nr);
    calculate(100, 16, &timing_values, &mutex_next_nr);

    printf("Starting Calculations...\n");

    for(int i = 0; i < n_tests; i++){
        mc_results[i] = calculate(test_n[i], cores, &timing_values, &mutex_next_nr);
        if(mc_results[i] > 200){
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

    printf("Multicore:\n");
    if(file){
        fprintf(file, "Multicore results:\n");
    }

    for(int i = 0; i < n_tests; i++){
        if(!mc_results[i]){
            break;
        }
        printf("n=%d: %lf\n", test_n[i], mc_results[i]);
        if(file){
            fprintf(file, "n=%d: %lf\n", test_n[i], mc_results[i]);
        }
    }

    fclose(file);

    return 0;
}