# Matrix Multiplication
I wanted to test various methods of multiplying matrices together.

## Methods
### Sequentially
```C
void sequential(long long N, int* mat1, int* mat2, int* mat3) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            mat3[i*N+j] = 0;
            for (int k = 0; k < N; k++)
                mat3[i*N+j] += mat1[i*N+k] * mat2[k*N+j];
        }
    }
}
```
### Parallelized on the CPU
```C
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
        #pragma omp barrier
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                for (int k = thread_num; k < N; k+=num_threads) {
                    #pragma omp atomic
                    mat3[i*N+j] += mat1[i*N+k] * mat2[k*N+j];
                }
            }
        }                   
    }
}
```
### Parallelized on the GPU
```
#version 460

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

uniform int N;

layout (std430, binding = 0) readonly buffer MatrixA {
    int A[];
};

layout (std430, binding = 1) readonly buffer MatrixB {
    int B[];
};

layout (std430, binding = 2) writeonly buffer MatrixC {
    int C[];
};

void main() {
    uint row = gl_GlobalInvocationID.y;
    uint col = gl_GlobalInvocationID.x;

    if (row >= N || col >= N)
        return;

    C[row * N + col] = 0;
    for (int i = 0; i < N; i++) {
        C[row * N + col] += A[row * N + i] * B[i * N + col];
    }
}
```
## Results
![Matrix Multiplication](https://github.com/user-attachments/assets/0cde5d19-9a8a-452f-9626-4833d71defea)
