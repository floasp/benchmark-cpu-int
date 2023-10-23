#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <pthread.h>
#include <sys/time.h>

#define BATCH_INCREASE 1  // 1 works best on machines tested so far

typedef struct batch_struct {
    int batch_id; // number of batch
    int n_max; // numbers that a will be increased by
    int *next; // next batch start
    int n_batches; // after finishing store number of batches used by this thread
    pthread_mutex_t* mutex_next_nr;
} batchsetup_t;

typedef struct timings_struct{
    struct timeval start, end;
    clock_t c_begin;
    clock_t c_end;
    double time;
    double time_;
} timings_t;

typedef struct benchmark_results_truct{
    int n_benchmarks; // amount of benchmarks to do
    int* goals; // number to calculate up to for each benchmark
    double* results; // resulting time for each benchmark
} benchmark_results_t;

int get_next_start_nr_asynch(int* next, pthread_mutex_t* mutex);
void *calculateBatch(void *vargp);
double calculate(int* start_n, int* end_n, int cores, timings_t* timing_values, pthread_mutex_t* mutex_next_nr);


#endif /* BENCHMARK_H */
