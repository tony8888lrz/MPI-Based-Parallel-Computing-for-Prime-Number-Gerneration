---

# MPI-Based Parallel Computing for Prime Number Generation

This project enhances a C-programmed, MPI-based parallel algorithm for prime number generation. By implementing block decomposition techniques, it improves data retrieval and cache hit rates, minimizes communication overhead, and leverages synchronous computation on multi-core systems. Benchmark tests conducted on a high-performance computing (HPC) cluster demonstrated significant performance improvements, reducing execution time from **17.73 seconds to 0.11 seconds**.

---

## Prerequisites
To run this project, you need access to an HPC cluster (e.g., `HPC-001`) or a similar environment with at least **160 processors**. Before running the program, load the required modules:

```bash
module load gcc/gcc-5.1.0
module load mpich-3.2.1/gcc-4.8.5
```

---

## Usage
1. Clone the repository:
   ```bash
   git clone https://github.com/tony8888lrz/MPI-Based-Parallel-Computing-for-Prime-Number-Gerneration
   cd MPI-Based-Parallel-Computing-for-Prime-Number-Gerneration
   ```
2. Start computation on the node:
   ```bash
   python3 starter.py
   ```
3. Monitor computation node usage:
   ```bash
   squeue -u $USER
   ```

---
