#ifndef MATMUL_H
#define MATMUL_H

/* all have signature long long, int*, int*, int* */

void sequential();
void openmp1();
void openmp2();
void openmp3();
void opengl();

/* --------------------------------------------- */

int initialize_compute_shader(void);
void destroy_compute_shader(void);

#endif