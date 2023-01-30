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

#define BATCH_INCREASE 1

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct batch_struct {
    int batch_id; // number of batch
    int n_max; // numbers that a will be increased by
    int *next; // next batch start
    int n_batches; // after finishing store number of batches used by this thread
} batchsetup_t;

int get_next_start(int* next){
    int n = 0;
    pthread_mutex_lock(&mutex);

    n = *next;
    *next += BATCH_INCREASE;

    pthread_mutex_unlock(&mutex);
    return n;
}

void *calculateBatch(void *vargp){

    batchsetup_t *args = (batchsetup_t *)vargp;
    int batch_id = args->batch_id;
    int n_max = args->n_max;
    int* next = args->next;

    int thread_batch_counter = 0;

    while(1){
        int n = get_next_start(next);

        if(n >= n_max)
            break;
        
        thread_batch_counter++;

        int start_a = (n > 3) ? (n) : 3;
        int max_a = (n + BATCH_INCREASE < n_max) ? (n + BATCH_INCREASE) : n_max;

        for(long long a = start_a; a < max_a; a++){
            for(long long b = 2; b < a; b++){
                for(long long c = 1; c <= b; c++){
                    
                    long long denominator = a*b*c;
                    long long a_ac = a*a*c;
                    long long b_ab = b*b*a;
                    long long c_bc = c*b*c;
                    long long sum = a_ac + b_ab + c_bc;

                    if(sum % denominator == 0){
                        long long result = sum / denominator;
                        printf("thread %03d, batch %03d: %ld, %ld, %ld, %ld \n", batch_id, thread_batch_counter, a, b, c, result);
                    }
                }
            }
        }
    }

    args->n_batches = thread_batch_counter;

    return NULL;
}

double calculate(int n, int cores){

    int next_a = 3;
    batchsetup_t *batch_nrs = (batchsetup_t *)malloc(sizeof(batchsetup_t) * cores);

    if (!batch_nrs){
        printf("Memory error!\n");
        return 0;
    }

    for(int i = 0; i < cores; i++){
        batch_nrs[i].batch_id = i;
        batch_nrs[i].n_max = n;
        batch_nrs[i].next = &next_a;
    }
    printf("Start with n=%d.\n", n);
    
    struct timeval start, end;
    gettimeofday(&start, NULL);
    clock_t c_begin = clock();

    pthread_t tid[cores];
    for (int i = 0; i < cores; i++){
        pthread_create(&tid[i], NULL, calculateBatch, (void *)&batch_nrs[i]);
    }
    for (int i = 0; i < cores; i++){
       pthread_join(tid[i], NULL);
    }
    
    gettimeofday(&end, NULL);
    clock_t c_end = clock();

    for(int i = 0; i < cores; i++){
        printf("thread %03d: %d batches.\n", i, batch_nrs[i].n_batches);
    }

    free(batch_nrs);

    double time = (double)(c_end-c_begin) / CLOCKS_PER_SEC;
    long seconds = (end.tv_sec - start.tv_sec);
    long micros = (end.tv_usec) - (start.tv_usec);
    double time_ = seconds + (double)micros / 1000000;
    printf("Finished n=%d.\n", n);
    printf("Runtime on CPU: %lfs (divide by N on Linux)\n", time);
    printf("Wall Clock Time: %lfs\n", time_);
    printf("\n");

    return time_;
}

int main(){
    printf("Starting Benchmark!\n");

    int cores = getNumberOfCores();
    printf("Number of logical CPU cores: %d\n", cores);

    int n_tests = 7;
    //int test_n[7] = {1280, 2560, 5120, 10240, 20480, 40960, 81920};
    int test_n[7] = {1000, 2000, 5000, 10000, 20000, 50000, 100000};
    double sc_results[7] = {0};
    double mc_results[7] = {0};

    calculate(100, 1);
    calculate(100, 16);

    printf("Starting Calculations...\n");

    for(int i = 0; i < n_tests; i++){
        mc_results[i] = calculate(test_n[i], cores);
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