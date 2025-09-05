/*
Compilación:
    gcc AppliedThreads.c -O2 -pthread -o matmul

Ejecución (ejemplo con matriz de 5x5 usando 4 threads):
    ./matmul 5 4
    
Para usar todos los CPU cores disponibles, usar 0 como segundo argumento:
    ./matmul 5 0
*/

#define _POSIX_C_SOURCE 199309L
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <pthread.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

static const int RAND_RANGE = 100;

typedef struct {
    int *A;
    int *B;
    int *C;
    int N;
    int start_row;
    int end_row;
} thread_data_t;

void* matrix_multiply_thread(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    int N = data->N;
    int* A = data->A;
    int* B = data->B;
    int* C = data->C;
    
    for (int i = data->start_row; i < data->end_row; ++i) {
        for (int k = 0; k < N; ++k) {
            int aik = A[(size_t)i * N + k];
            for (int j = 0; j < N; ++j) {
                C[(size_t)i * N + j] += aik * B[(size_t)k * N + j];
            }
        }
    }
    
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        return 1;
    }
    
    int N = atoi(argv[1]);
    int requested_threads = atoi(argv[2]);
    
    if (N <= 0) {
        return 1;
    }
    
    if (requested_threads < 0) {
        return 1;
    }

    size_t nn = (size_t)N * (size_t)N;
    if (nn > SIZE_MAX / sizeof(int)) return 1;

    int *A = (int *)malloc(nn * sizeof(int));
    int *B = (int *)malloc(nn * sizeof(int));
    int *C = (int *)malloc(nn * sizeof(int));
    if (!A || !B || !C) {
        free(A); free(B); free(C);
        return 1;
    }

    srand((unsigned)time(NULL));

    for (size_t i = 0; i < nn; ++i) {
        A[i] = rand() % RAND_RANGE;
        B[i] = rand() % RAND_RANGE;
        C[i] = 0;
    }

    // Determine number of threads to use
    int num_threads;
    if (requested_threads == 0) {
        // Auto-detect CPU cores
#ifdef _WIN32
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        num_threads = sysinfo.dwNumberOfProcessors;
#else
        num_threads = sysconf(_SC_NPROCESSORS_ONLN);
#endif
        if (num_threads <= 0) num_threads = 4; // fallback
    } else {
        num_threads = requested_threads;
    }

    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    thread_data_t* thread_data = malloc(num_threads * sizeof(thread_data_t));
    
    if (!threads || !thread_data) {
        free(A); free(B); free(C); free(threads); free(thread_data);
        return 1;
    }

    // Timing variables
    struct timespec start, end;

    // Start timing
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Create threads with row distribution
    for (int t = 0; t < num_threads; ++t) {
        thread_data[t].A = A;
        thread_data[t].B = B;
        thread_data[t].C = C;
        thread_data[t].N = N;
        
        // Distribute rows evenly among threads
        thread_data[t].start_row = t * N / num_threads;
        thread_data[t].end_row = (t + 1) * N / num_threads;
        
        // Last thread handles any remaining rows due to integer division
        if (t == num_threads - 1) {
            thread_data[t].end_row = N;
        }
        
        pthread_create(&threads[t], NULL, matrix_multiply_thread, &thread_data[t]);
    }

    // Wait for all threads to complete
    for (int t = 0; t < num_threads; ++t) {
        pthread_join(threads[t], NULL);
    }

    // End timing and calculate elapsed time
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) * 1e3 +
                     (end.tv_nsec - start.tv_nsec) / 1e6; // milisegundos
    printf("%.3f ms\n", elapsed);

    free(threads);
    free(thread_data);
    free(A);
    free(B);
    free(C);
    return 0;
}