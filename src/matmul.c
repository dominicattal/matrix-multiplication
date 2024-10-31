#include "matmul.h"
#include <omp.h>

void sequential(long long N, int* mat1, int* mat2, int* mat3) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            mat3[i*N+j] = 0;
            for (int k = 0; k < N; k++)
                mat3[i*N+j] += mat1[i*N+k] * mat2[k*N+j];
        }
    }
}

void openmp1(long long N, int* mat1, int* mat2, int* mat3) {
    #pragma omp parallel
    {
        int num_threads = omp_get_num_threads();
        int thread_num = omp_get_thread_num();
        for (int i = thread_num; i < N; i+=num_threads) {
            for (int j = 0; j < N; j++) {
                mat3[i*N+j] = 0;
                for (int k = 0; k < N; k++)
                    mat3[i*N+j] += mat1[i*N+k] * mat2[k*N+j];
            }
        }
    }
}

void openmp2(long long N, int* mat1, int* mat2, int* mat3) {
    #pragma omp parallel
    {
        int num_threads = omp_get_num_threads();
        int thread_num = omp_get_thread_num();
        for (int i = 0; i < N; i++) {
            for (int j = thread_num; j < N; j+=num_threads) {
                mat3[i*N+j] = 0;
                for (int k = 0; k < N; k++)
                    mat3[i*N+j] += mat1[i*N+k] * mat2[k*N+j];
            }
        }
    }
}

void openmp3(long long N, int* mat1, int* mat2, int* mat3) {
    #pragma omp parallel
    {
        int num_threads = omp_get_num_threads();
        int thread_num = omp_get_thread_num();
        for (int i = thread_num; i < N * N; i += num_threads)
            mat3[i] = 0;
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                for (int k = thread_num; k < N; k+=num_threads)
                    mat3[i*N+j] += mat1[i*N+k] * mat2[k*N+j];
    }
}

void opengl(long long N, int* mat1, int* mat2, int* mat3) {
   
}
