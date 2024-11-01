#include <stdio.h>
#include <glad.h>
#include <glfw.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "matmul.h"

#define MAX_N 10000
#define MAX_ELE 20

#define MATS_FLAG 0
#define SEQUENTIAL_FLAG 1
#define OPENMP1_FLAG 2
#define OPENMP2_FLAG 3
#define OPENMP3_FLAG 4
#define OPENGL_FLAG 5
#define NUM_FLAGS 6

void mat_dump(long long N, int* mat, char* name) {
    FILE* fptr = fopen(name, "w");
    if (fptr == NULL) {
        puts("Something went wrong");
        return;
    }
    printf("Creating matrix dump in %s...\n", name);
    fprintf(fptr, "N = %-d\n", N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            fprintf(fptr, "%-10d ", mat[N*i+j]);
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
    printf("Finished in %.6f seconds\n", sec);
}

#define CMP(arg, set, flag) \
    if (!strcmp(arg, argv[i])) \
        flags[flag] = set;

void set_flag(int* flags, char** argv, int i) {
    CMP("-d", 1, MATS_FLAG);
    CMP("-s", 1, SEQUENTIAL_FLAG);
    CMP("-sd", 2, SEQUENTIAL_FLAG);
    CMP("-mp1", 1, OPENMP1_FLAG);
    CMP("-mp1d", 2, OPENMP1_FLAG);
    CMP("-mp2", 1, OPENMP2_FLAG);
    CMP("-mp2d", 2, OPENMP2_FLAG);
    CMP("-mp3", 1, OPENMP3_FLAG);
    CMP("-mp3d", 2, OPENMP3_FLAG);
    CMP("-gl", 1, OPENGL_FLAG);
    CMP("-gld", 2, OPENGL_FLAG);
}

int parse_input(long long* N, int* flags, int argc, char** argv) {
    int default_flag = 1;
    *N = -1;
    for (int i = 0; i < NUM_FLAGS; i++)
        flags[i] = 0;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            set_flag(flags, argv, i);
            if (!strcmp("-D", argv[i])) {
                default_flag = 2;
                flags[MATS_FLAG] = 1;
            }
        } else {
            *N = atol(argv[i]);
            printf("N = %lld\n", *N);
            if (*N <= 0 || *N > MAX_N) {
                printf("Input a valid number between 1 and %lld\n", MAX_N);
                return 1;
            }
        }
    }
    if (*N == -1) {
        puts("Did not input value for N");
        return 0;
    }

    for (int i = 1; i < NUM_FLAGS; i++)
        if (flags[i])
            return 1;

    for (int i = 1; i < NUM_FLAGS; i++)
        flags[i] = default_flag;
    return 1;
}

int main(int argc, char** argv) {

    if (argc == 1) {
        puts("Input matrix size");
        return 1;
    }

    long long N;
    int flags[NUM_FLAGS];
    if (!parse_input(&N, flags, argc, argv))
        return 1;

    if (!glfwInit()) {
        puts("Failed to initialize glfw");
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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

    printf("Loaded glad successfully\n");

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

    puts("Allocated matrices successfully");
    srand(time(NULL));

    if (flags[OPENGL_FLAG]) {
        if (!initialize_compute_shaders()) {
            puts("Failed to initalize compute shaders");
            free(mat1);
            free(mat2);
            free(mat3);
            glfwDestroyWindow(context);
            glfwTerminate();
            return 1;
        }
        printf("Initalized compute shader successfully\n\n");
    }

    run(&fill_mats, N, mat1, mat2, mat3, "Filling matrices with random numbers...");
    if (flags[MATS_FLAG]) {
        mat_dump(N, mat1, "mats/mat1.txt");
        mat_dump(N, mat2, "mats/mat2.txt");
    }
    puts("");

    if (flags[SEQUENTIAL_FLAG]) {
        run(&sequential, N, mat1, mat2, mat3, "Running sequential multiplcation...");
        if (flags[SEQUENTIAL_FLAG] == 2)
            mat_dump(N, mat3, "mats/sequential.txt");
        puts("");
    }

    if (flags[OPENMP1_FLAG]) {
        run(&openmp1, N, mat1, mat2, mat3, "Running CPU parallelized multiplcation...");
        if (flags[OPENMP1_FLAG] == 2)
            mat_dump(N, mat3, "mats/cpu1.txt");
        puts("");
    }

    if (flags[OPENMP2_FLAG]) {
        run(&openmp2, N, mat1, mat2, mat3, "Running CPU parallelized multiplcation...");
        if (flags[OPENMP2_FLAG] == 2)
            mat_dump(N, mat3, "mats/cpu2.txt");
        puts("");
    }


    if (flags[OPENMP3_FLAG]) {
        run(&openmp3, N, mat1, mat2, mat3, "Running CPU parallelized multiplcation...");
        if (flags[OPENMP3_FLAG] == 2)
            mat_dump(N, mat3, "mats/cpu3.txt");
        puts("");
    }

    if (flags[OPENGL_FLAG]) {
        run(&opengl, N, mat1, mat2, mat3, "Running GPU parallelized multiplcation...");
        if (flags[OPENGL_FLAG] == 2)
            mat_dump(N, mat3, "mats/gpu.txt");
        puts("");
    }

    free(mat1);
    free(mat2);
    free(mat3);
    destroy_compute_shaders();
    glfwDestroyWindow(context);
    glfwTerminate();
    return 0;
}