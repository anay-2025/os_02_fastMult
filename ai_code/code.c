#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // malloc(), rand(), srand()
#include <unistd.h>     // fork()
#include <sys/wait.h>   // wait()
#include <sys/mman.h>   // mmap() for shared memory
#include <time.h>       // time(), clock()

#define CHILDREN 8      // Number of child processes (forks)

/*
 Allocate a normal matrix in contiguous memory (1D array)
 Used for A, B, BT, and sequential result
*/
int *alloc_matrix(int n) {
    return malloc(n * n * sizeof(int));
}

/*
 Allocate shared memory matrix so all forked processes
 can write to the same result matrix
*/
int *alloc_shared_matrix(int n) {
    return mmap(NULL,
                n * n * sizeof(int),
                PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS,
                -1, 0);
}

/*
 Fill matrix with random values (0–9)
*/
void fill(int *M, int n) {
    for (int i = 0; i < n * n; i++)
        M[i] = rand() % 10;
}

/*
 Transpose matrix B into BT
 Improves cache locality during multiplication
*/
void transpose(int *B, int *BT, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            BT[j * n + i] = B[i * n + j];
}

/*
 Sequential matrix multiplication
 C = A × Bᵀ
*/
void seqMultiply(int *A, int *BT, int *C, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            int sum = 0;
            for (int k = 0; k < n; k++)
                sum += A[i * n + k] * BT[j * n + k];
            C[i * n + j] = sum;
        }
}

/*
 Parallel matrix multiplication using fork()
 Each child computes a block of rows
*/
void parMultiply(int *A, int *BT, int *C, int n) {
    int rows = n / CHILDREN;   // Rows per child

    for (int c = 0; c < CHILDREN; c++) {
        pid_t pid = fork();    // Create child process

        if (pid == 0) {        // Child process
            int start = c * rows;
            int end = (c == CHILDREN - 1) ? n : start + rows;

            // Compute assigned rows
            for (int i = start; i < end; i++)
                for (int j = 0; j < n; j++) {
                    int sum = 0;
                    for (int k = 0; k < n; k++)
                        sum += A[i * n + k] * BT[j * n + k];
                    C[i * n + j] = sum;
                }
            exit(0);   // Child exits after work is done
        }
    }

    // Parent waits for all children to finish
    for (int i = 0; i < CHILDREN; i++)
        wait(NULL);
}

int main() {
    int n;
    printf("Enter matrix size n -> ");
    scanf("%d", &n);

    srand(time(NULL));   // Seed random number generator

    // Allocate matrices
    int *A  = alloc_matrix(n);          // Matrix A
    int *B  = alloc_matrix(n);          // Matrix B
    int *BT = alloc_matrix(n);          // Transposed B
    int *C1 = alloc_matrix(n);          // Sequential result
    int *C2 = alloc_shared_matrix(n);   // Parallel result (shared)

    // Initialize matrices
    fill(A, n);
    fill(B, n);
    transpose(B, BT, n);

    clock_t start, end;

    // Sequential execution timing
    start = clock();
    seqMultiply(A, BT, C1, n);
    end = clock();
    double st = (double)(end - start) / CLOCKS_PER_SEC;

    // Parallel execution timing
    start = clock();
    parMultiply(A, BT, C2, n);
    end = clock();
    double pt = (double)(end - start) / CLOCKS_PER_SEC;

    // Display execution times
    printf("\nSequential Time : %f sec", st);
    printf("\nParallel Time   : %f sec\n", pt);

    return 0;
}
