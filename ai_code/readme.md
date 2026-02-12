# Parallel vs. Sequential Matrix Multiplication

A high-performance C implementation comparing sequential matrix multiplication with process-based parallelism using `fork()` and shared memory (`mmap`).

---

## 🚀 Overview

This project explores the performance gap between single-threaded execution and multi-process execution for large-scale matrix operations. By leveraging shared memory and cache optimization techniques (like matrix transposition), the program efficiently multiplies square matrices of size \( n \times n \) (where \( n \geq 10{,}000 \)).

---

## 🛠️ Key Features

### Process-Based Parallelism
Uses `fork()` to distribute row-wise computations across multiple child processes.

### Shared Memory
Implements `mmap()` to create a shared memory region, allowing child processes to write to the result matrix `C` simultaneously.

### Cache Optimization
Includes a Matrix Transpose step for Matrix `B` to ensure row-major access patterns, significantly reducing cache misses.

### Memory Efficiency
Matrices are stored as 1D contiguous arrays to minimize virtual memory overhead and fragmentation.

---

## 🏗️ Architecture & Implementation

### 1. Matrix Initialization

Matrices are dynamically allocated and populated with random integers (0–9) using:

```c
srand(time(NULL));
```

- Matrix A & B: Allocated via `malloc()`
- Matrix C (Result): Allocated via `mmap()` with `MAP_SHARED | MAP_ANONYMOUS` flags to allow inter-process communication

---

### 2. Parallel Strategy

The workload is split by rows.

If there are \( M \) processes and \( N \) rows:

- Each child process is assigned a specific range of rows.
- The parent process manages the lifecycle using `wait()`, ensuring all chunks are completed before the final time measurement.

---

### 3. The Transpose Optimization

To avoid the "stride-N" access pattern during multiplication, Matrix \( B \) is transposed to \( B^T \).

**Why?**

Instead of jumping across memory to read columns, the CPU reads contiguous memory blocks (rows), maximizing L1/L2 cache hits.

---

## 📊 Performance Analysis

The project measures Wall-Clock Time using `clock()` to compare the two methods.

| Metric | Sequential | Parallel (fork) |
|--------|------------|----------------|
| Workload Distribution | Single Process | Row-wise Chunks |
| Memory Access | Contiguous (with Transpose) | Shared Memory (mmap) |
| Scaling | \( O(n^3) \) | \( \approx O(n^3 / \text{cores}) \) |

Observation: Peak performance is generally observed when the number of child processes matches the available physical CPU cores.

---

## 💻 How to Run

### Clone the repository:

```bash
git clone https://github.com/yourusername/matrix-mult-parallel.git
cd matrix-mult-parallel
```

### Compile:

```bash
gcc -O3 matrix_mult.c -o matrix_mult
```

### Execute:

```bash
./matrix_mult
```
