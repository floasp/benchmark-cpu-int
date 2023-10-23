#include "benchmark.h"
#include <stdio.h>
#include <stdlib.h>

int get_next_start_nr_asynch(int* next, pthread_mutex_t* mutex_next_nr){
    int n = 0;
    pthread_mutex_lock(mutex_next_nr);

    n = *next;
    *next += BATCH_INCREASE;

    pthread_mutex_unlock(mutex_next_nr);
    return n;
}

void *calculateBatch(void *vargp){

    batchsetup_t *args = (batchsetup_t *)vargp;
    int batch_id = args->batch_id;
    int n_max = args->n_max;
    int* next = args->next;
    pthread_mutex_t* mutex_next_nr = args->mutex_next_nr;

    int thread_batch_counter = 0;

    while(1){
        int n = get_next_start_nr_asynch(next, mutex_next_nr);

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

double calculate(int* start_n, int* end_n, int cores, timings_t* timing_values, pthread_mutex_t* mutex_next_nr){
    int next_a = *start_n;
    if (next_a < 3){
        next_a = 3;  // lower than 3 doesn't make sense
        *start_n = 3;
        printf("Starting value too low (has to be bigger than or equal to 3). Start value set to: %d.\n", *start_n);
    }
    if (*end_n <= next_a){
        *end_n = next_a + 1;  // end can't be lower than start
        printf("End value too low (has to be bigger than or equal to start value +1). End value set to: %d.\n", *end_n);
    }
    batchsetup_t *batch_nrs = (batchsetup_t *)malloc(sizeof(batchsetup_t) * cores);

    if (!batch_nrs){
        printf("Memory error!\n");
        return 0;
    }

    for(int i = 0; i < cores; i++){
        batch_nrs[i].batch_id = i;
        batch_nrs[i].n_max = *end_n;
        batch_nrs[i].next = &next_a;
        batch_nrs[i].mutex_next_nr = mutex_next_nr;
    }
    //printf("Start with n=%d.\n", end_n);
    printf("Start with (%d -> %d)\n", *start_n, *end_n);

    gettimeofday(&(timing_values->start), NULL);
    timing_values->c_begin = clock();

    pthread_t tid[cores];
    for (int i = 0; i < cores; i++){
        pthread_create(&tid[i], NULL, calculateBatch, (void *)&batch_nrs[i]);
    }
    for (int i = 0; i < cores; i++){
        pthread_join(tid[i], NULL);
    }
    
    gettimeofday(&timing_values->end, NULL);
    timing_values->c_end = clock();

    for(int i = 0; i < cores; i++){
        printf("thread %03d: %d batches.\n", i, batch_nrs[i].n_batches);
    }

    free(batch_nrs);

    timing_values->time = (double)(timing_values->c_end - timing_values->c_begin) / CLOCKS_PER_SEC;
    long seconds = (timing_values->end.tv_sec - timing_values->start.tv_sec);
    long micros = (timing_values->end.tv_usec) - (timing_values->start.tv_usec);
    timing_values->time_ = seconds + (double)micros / 1000000;
    printf("Finished (%d -> %d).\n", *start_n, *end_n);
    printf("Runtime on CPU: %lfs (divide by N on Linux)\n", timing_values->time);
    printf("Wall Clock Time: %lfs\n", timing_values->time_);
    printf("\n");

    return timing_values->time_;
}

void benchmark(){

}
