# Parallel K-Means Clustering with CUDA

This repository significantly accelerates K-Means clustering using CUDA, achieving up to **74.46x speedup** in our experiments.

### Key Improvements:
- **Speedup:** Up to 74.46x faster compared to baseline implementations.
- **Time Complexity Reduction:** Reduced from **O(N*K*T)** to **O(T(K+a))**, where **a** and **K** are small constants.
- **Efficiency:** The program is no longer computation-bound due to CUDA optimizations.

For a detailed introduction, implementation, and evaluation, please refer to the project report: [**K-means-parallel**](https://github.com/ryan910707/k-means-parallel/blob/master/k-means-parallel.pdf).

---
This project extends the work from [vinayak1998/Parallel-K-Means-Clustering](https://github.com/vinayak1998/Parallel-K-Means-Clustering) by optimizing it using CUDA.

### Implementations:
- **Sequential**
- **OpenMP**
- **P-Threads**
- **CUDA (optimized)**

---

## Getting Started

### CUDA Implementation
   ```bash
   cd CUDA
   make
   ```
- we offer 3 script:
  - correctness.sh for testing the correctness
  - k_perf.sh for testing diff K
  - performance.sh for testing diff N
  - please refer to the bash file for sample execution

- **Input**
  - we also offer an input generator in Input folder for generating the input data.
  ```bash
  ./input_generator N 
  ```
- for other version please refer to the root author's repository
 

