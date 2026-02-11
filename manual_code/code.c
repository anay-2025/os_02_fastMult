#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define CHILDREN 8   // default 8 forks

int** create(int n) {
    int **mat = (int**) malloc(n * sizeof(int*));
    for(int i = 0; i < n; i++) {
        mat[i] = (int*) malloc(n * sizeof(int));
    }
    return mat;
}

void fill(int **mat, int n) {
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            mat[i][j] = random() % 10;
        }
    }
}

// with 0 forks
void seqMultiply(int **A, int **B, int **C, int n) {
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            C[i][j] = 0;
            for(int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// with 8 froks
void parMultiply(int **A, int **B, int **C, int n) {
    int rows = n / CHILDREN;
    for(int c = 0; c < CHILDREN; c++) {
        pid_t pid = fork();
        if(pid == 0) {   
            int start = c * rows;
            int end   = (c == CHILDREN - 1) ? n : start + rows;
            for(int i = start; i < end; i++) {
                for(int j = 0; j < n; j++) {
                    C[i][j] = 0;
                    for(int k = 0; k < n; k++) {
                        C[i][j] += A[i][k] * B[k][j];
                    }
                }
            }
            exit(0);  
        }
    }
    for(int i = 0; i < CHILDREN; i++) {
        wait(NULL);
    }
}

void freeSpace(int **mat, int n) {
    for(int i = 0; i < n; i++) {
        free(mat[i]);
    }
    free(mat);
}

int main() {
    int n;
    printf("Enter matrix size n for (n x n) matrix -> ");
    scanf("%d", &n);

    srand(time(NULL));
    srandom(time(NULL));

    int **A  = create(n);
    int **B  = create(n);
    int **C1 = create(n);   
    int **C2 = create(n);   

    fill(A, n);
    fill(B, n);

    clock_t start, end;

    // with 0 forks
    start = clock();
    seqMultiply(A, B, C1, n);
    end = clock();

    double st = (double)(end - start) / CLOCKS_PER_SEC;

    // with 8 forks
    start = clock();
    parMultiply(A, B, C2, n);
    end = clock();

    double pt = (double)(end - start) / CLOCKS_PER_SEC;

    printf("\nSequential Time with no forks = %f sec\n", st);
    printf("\nParallel Time with default 8 forks = %f sec\n", pt);

    freeSpace(A, n);
    freeSpace(B, n);
    freeSpace(C1, n);
    freeSpace(C2, n);

    return 0;
}
