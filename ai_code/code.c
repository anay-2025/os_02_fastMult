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
