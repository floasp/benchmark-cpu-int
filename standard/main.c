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

typedef struct arg_struct {
    int batch;
    int cores;
    int n;
} args_t;

void *calculateBatch(void *vargp){

    args_t *args = (args_t *)vargp;
    int batch = args->batch;
    int cores = args->cores;
    int n = args->n;

    for(long long a = 3 + batch; a < n; a+=cores){
        for(long long b = 2; b < a; b++){
            for(long long c = 1; c <= b; c++){
                
                long long denominator = a*b*c;
                long long a_ac = a*a*c;
                long long b_ab = b*b*a;
                long long c_bc = c*b*c;
                long long sum = a_ac + b_ab + c_bc;

                if(sum % denominator == 0){
                    long long result = sum / denominator;
                    printf("%ld, %ld, %ld, %ld \n", a, b, c, result);
                }
            }
        }
    }

    return NULL;
}

double calculate(int n, int cores){

    args_t *batch_nrs = (args_t *)malloc(sizeof(args_t) * cores);

    if (!batch_nrs){
        printf("Memory error!\n");
        return 0;
    }

    for(int i = 0; i < cores; i++){
        batch_nrs[i].batch = i;
        batch_nrs[i].cores = cores;
        batch_nrs[i].n = n;
    }
    printf("Start with n=%ld.\n", n);
    
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

    free(batch_nrs);

    double time = (double)(c_end-c_begin) / CLOCKS_PER_SEC;
    long seconds = (end.tv_sec - start.tv_sec);
    long micros = (end.tv_usec) - (start.tv_usec);
    double time_ = seconds + (double)micros / 1000000;
    printf("Finished n=%ld.\n", n);
    printf("Runtime on CPU: %lfs (divide by N on Linux)\n", time);
    printf("Wall Clock Time: %lfs\n", time_);
    printf("\n");

    return time_;
}

int main(){
    printf("Starting Benchmark!\n");

    int cores = getNumberOfCores();
    printf("Number of logical CPU cores: %d\n", cores);

    int n_tests = 10;
    int test_n[10] = {100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000};
    double sc_results[10] = {0};
    double mc_results[10] = {0};

    calculate(100, 1);
    calculate(100, 16);

    printf("Starting Calculations...\n");
    printf("Singlecore:\n");

    for(int i = 0; i < n_tests; i++){
        sc_results[i] = calculate(test_n[i], 1);
        if(sc_results[i] > 200){
            break;
        }
        printf("Cooldown...\n");

        my_sleep(1);
    }

    printf("Multicore:\n");

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
        fprintf(file, "Singlecore results:\n");
    }

    printf("Singlecore:\n");
    for(int i = 0; i < n_tests; i++){
        if(!sc_results[i]){
            break;
        }
        printf("n=%ld: %lf\n", test_n[i], sc_results[i]);
        if(file){
            fprintf(file, "n=%ld: %lf\n", test_n[i], sc_results[i]);
        }
    }
    printf("Multicore:\n");
    if(file){
        fprintf(file, "Multicore results:\n");
    }

    for(int i = 0; i < n_tests; i++){
        if(!mc_results[i]){
            break;
        }
        printf("n=%ld: %lf\n", test_n[i], mc_results[i]);
        if(file){
            fprintf(file, "n=%ld: %lf\n", test_n[i], mc_results[i]);
        }
    }

    fclose(file);

    return 0;
}
