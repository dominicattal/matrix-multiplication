#include <stdio.h>
#include <glad.h>
#include <glfw.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "matmul.h"

#define MAX_N 10000
#define MAX_ELE 20

void mat_dump(long long N, int* mat, char* name) {
    FILE* fptr = fopen(name, "w");
    if (fptr == NULL) {
        puts("Something went wrong");
        exit(1);
    }
    printf("Creating matrix dump in %s...\n", name);
    fprintf(fptr, "N = %-d\n", N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(fptr, "%-10d ", mat[N*i+j]);
        }
        fprintf(fptr, "\n");
    }
    fclose(fptr);
}

void fill_mats(long long N, int* mat1, int* mat2, int* mat3) {
    for (long long i = 0; i < N; i++) {
        for (long long j = 0; j < N; j++) {
            mat1[N*i+j] = rand() % MAX_ELE;
            mat2[N*i+j] = rand() % MAX_ELE;
        }
    }
}

void run(void (*func)(), long long N, int* mat1, int* mat2, int* mat3, char* message) {
    struct timeval start, end;
    double sec;
    puts(message);
    gettimeofday(&start, NULL);
    func(N, mat1, mat2, mat3);
    gettimeofday(&end, NULL);
    sec = ((end.tv_sec - start.tv_sec)*1e6 + end.tv_usec - start.tv_usec)*1e-6;
    printf("Finished in %.6f seconds\n\n", sec);
}

int main(int argc, char** argv) {

    if (argc == 1) {
        puts("Input matrix size");
        return 1;
    }

    long long N = atol(argv[1]);
    printf("N = %lld\n", N);
    if (N <= 0 || N > MAX_N) {
        printf("Input a valid number between 1 and %lld\n", MAX_N);
        return 1;
    }

    if (!glfwInit()) {
        puts("Failed to initialize glfw");
        return 1;
    }
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow* context = glfwCreateWindow(1,1,"offscreen", NULL, NULL);
    if (!context) {
        puts("Failed to make context");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(context);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        puts("Failed to make context current");
        glfwDestroyWindow(context);
        glfwTerminate();
        return 1;
    }

    printf("Loaded glad successfully\n\n");

    int* mat1 = malloc(N * N * sizeof(int));
    if (mat1 == NULL) {
        puts("Failed to allocate memory for mat1");
        glfwDestroyWindow(context);
        glfwTerminate();
        return 1;
    }

    int* mat2 = malloc(N * N * sizeof(int));
    if (mat2 == NULL) {
        puts("Failed to allocate memory for mat2");
        glfwDestroyWindow(context);
        glfwTerminate();
        return 1;
    }

    int* mat3 = malloc(N * N * sizeof(int));
    if (mat3 == NULL) {
        puts("Failed to allocate memory for mat3");
        glfwDestroyWindow(context);
        glfwTerminate();
        return 1;
    }

    srand(time(NULL));

    run(&fill_mats, N, mat1, mat2, mat3, "Filling matrices with random numbers...");
    run(&sequential, N, mat1, mat2, mat3, "Running sequential multiplcation...");
    run(&openmp1, N, mat1, mat2, mat3, "Running CPU parallelized multiplcation...");
    run(&openmp2, N, mat1, mat2, mat3, "Running CPU parallelized multiplcation...");
    run(&openmp3, N, mat1, mat2, mat3, "Running CPU parallelized multiplcation...");
    run(&opengl, N, mat1, mat2, mat3, "Running GPU parallelized multiplcation...");

    /* mat_dump(N, mat1, "mat1.txt");
    mat_dump(N, mat2, "mat2.txt");
    mat_dump(N, mat3, "mat3.txt"); */

    free(mat1);
    free(mat2);
    free(mat3);

    glfwDestroyWindow(context);
    glfwTerminate();
    return 0;
}