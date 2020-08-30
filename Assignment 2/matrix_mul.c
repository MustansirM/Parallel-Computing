#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

typedef double TYPE;
#define MIN 1
#define MAX 1000

void printMatrix(int, TYPE*, FILE*);
TYPE* generateMatrix(int);
TYPE* generateEmptyMatrix(int);
TYPE* matrixMulParOne(int, TYPE*, TYPE*, TYPE*, int);
TYPE* matrixMulParTwo(int, TYPE*, TYPE*, TYPE*, int);
TYPE* matrixMulParThree(int, TYPE*, TYPE*, TYPE*, int);

TYPE* generateMatrix(int dims){
    TYPE* M = (TYPE *)malloc(dims*dims*sizeof(TYPE));
    struct timeval t;
    gettimeofday(&t, NULL);
    unsigned long long u = t.tv_sec*1000000 + t.tv_usec;
    srand(u);
    for(int i=0; i<dims; ++i){
        for(int j=0; j<dims; ++j){
            M[i*dims+j] = (TYPE)(rand()%MAX+ MIN);
        }
    }
    return M;
}

TYPE* generateEmptyMatrix(int dims){
    return (TYPE *)calloc(dims*dims, sizeof(TYPE));
}

void clearMatrix(int dims, TYPE *C){
    for(int i=0; i<dims; ++i){
        for(int j=0; j<dims; ++j){
            C[i*dims+j] = 0;
        }
    }
}

TYPE* matrixMulParOne(int dims, TYPE* A, TYPE* B, TYPE* C, int threads){
    #pragma omp parallel for collapse(1) num_threads(threads)
    for(int i=0; i<dims; ++i){
        for(int j=0; j<dims; ++j){
            for(int k=0; k<dims; ++k){
                C[i*dims+j] += A[i*dims+k]*B[k*dims+j];
            }
        }
    }
    return C;
}

TYPE* matrixMulParTwo(int dims, TYPE* A, TYPE* B, TYPE* C, int threads){
    #pragma omp parallel for collapse(2) num_threads(threads)
    for(int i=0; i<dims; ++i){
        for(int j=0; j<dims; ++j){
            for(int k=0; k<dims; ++k){
                C[i*dims+j] += A[i*dims+k]*B[k*dims+j];
            }
        }
    }
    return C;
}

TYPE* matrixMulParThree(int dims, TYPE* A, TYPE* B, TYPE* C, int threads){
    #pragma omp parallel for collapse(3) num_threads(threads)
    for(int i=0; i<dims; ++i){
        for(int j=0; j<dims; ++j){
            for(int k=0; k<dims; ++k){
                C[i*dims+j] += A[i*dims+k]*B[k*dims+j];
            }
        }
    }
    return C;
}

void printMatrix(int dims, TYPE* M, FILE* fp){
    fprintf(fp, "Dimensions of matrix = %d %d\n", dims, dims);
    fprintf(fp, "Matrix: \n");
    for(int i=0; i<dims; ++i){
        for(int j=0; j<dims; ++j){
            fprintf(fp, "%.0f ", M[i*dims+j]);
        }
        fprintf(fp, "\n");
    }
}

int main(int argc, char* argv[]){
    int  dims, threads;
    unsigned long long delta_us;
    struct timeval start, end;
    FILE *input = fopen("generated_input.txt", "w");
    FILE *output = fopen("output.txt", "w");
    printf("Enter matrix dimensions\n");
    scanf("%d", &dims);
    printf("Enter number of threads\n");
    scanf("%d", &threads);

    TYPE *A = generateMatrix(dims);
    printMatrix(dims, A, input);
    TYPE *B = generateMatrix(dims);
    printMatrix(dims, B, input);
    fclose(input);

    TYPE *C = generateEmptyMatrix(dims);

    gettimeofday(&start, NULL);
    C = matrixMulParOne(dims, A, B, C, threads);
    gettimeofday(&end, NULL);
    delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    printf("Time taken if only outermost loop parallelized = %llu microseconds\n", delta_us);
    printMatrix(dims, C, output);
    clearMatrix(dims, C);

    gettimeofday(&start, NULL);
    C = matrixMulParTwo(dims, A, B, C, threads);
    gettimeofday(&end, NULL);
    delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    printf("Time taken if the two outer loops are parallelized = %llu microseconds\n", delta_us);
    printMatrix(dims, C, output);
    clearMatrix(dims, C);

    gettimeofday(&start, NULL);
    C = matrixMulParThree(dims, A, B, C, threads);
    gettimeofday(&end, NULL);
    delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    printf("Time taken if all threee loops are parallelized = %llu microseconds\n", delta_us);
    printMatrix(dims, C, output);

    fclose(output);
    return 0;
}