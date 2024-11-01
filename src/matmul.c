#include "matmul.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <glad.h>

static GLuint shader, program, bufferA, bufferB, bufferC;

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

void opengl(long long N, int* mat1, int* mat2, int* mat3) {
    size_t size = N * N * sizeof(int);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferA);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, mat1, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferB);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, mat2, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferC);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_STATIC_DRAW);
    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "N"), N);
    glDispatchCompute((N + 15) / 16, (N + 15) / 16, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size, mat3);
}

static char* read_file(char* path)
{
    FILE* ptr;
    char* content;
    ptr = fopen(path, "r");
    if (ptr == NULL) {
        printf("Could not open %s\n", path);
        return NULL;
    }
    fseek(ptr, 0, SEEK_END);
    int len = ftell(ptr);
    if (len == 0) {
        printf("File %s is empty\n", path);
        fclose(ptr);
        return NULL;
    }
    fseek(ptr, 0, SEEK_SET);
    content = calloc(len, sizeof(char));
    if (content == NULL) {
        printf("Could not allocate space to read %s\n", path);
        fclose(ptr);
        return NULL;
    }
    fread(content, 1, len, ptr);
    fclose(ptr);
    return content;
}

static int initialize_shader(GLuint success, char* info_log) {
    char* shader_code;
    shader = glCreateShader(GL_COMPUTE_SHADER);
    shader_code = read_file("src/matmul.comp");
    if (shader_code == NULL)
        return 0;
    glShaderSource(shader, 1, (const char* const*)&shader_code, NULL);
    free(shader_code);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        printf("Failed to compile shader\n%s", info_log);
        return 0;
    }
    return 1;
}

static int initialize_program(GLuint success, char* info_log) {
    program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        printf("Failed to link program\n%s", info_log);
        return 0;
    }
    glDetachShader(program, shader);
    glDeleteShader(shader);
    return 1;
}

int initialize_buffers(void) {
    glGenBuffers(1, &bufferA);
    glGenBuffers(1, &bufferB);
    glGenBuffers(1, &bufferC);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, bufferA);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, bufferB);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, bufferC);
    return 1;
}

int initialize_compute_shader(void) {
    GLuint success = 0;
    char info_log[512];
    if (!initialize_shader(success, info_log))
        return 0;    
    if (!initialize_program(success, info_log))
        return 0;
    if (!initialize_buffers())
        return 0;
    return 1;
}

void destroy_compute_shader(void) {
    glDeleteBuffers(1, &bufferA);
    glDeleteBuffers(1, &bufferB);
    glDeleteBuffers(1, &bufferC);
    glDeleteProgram(program);
}