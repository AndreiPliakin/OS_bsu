#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>

const int N = 1000;
constexpr int kMaxThreads = 10000;

static int A[N][N];
static int B[N][N];
static long long C[N][N];

void multiplyBlock(int row_start, int col_start, int block_size) {
    int rEnd = std::min(row_start + block_size, N);
    int cEnd = std::min(col_start + block_size, N);

    for (int i = row_start; i < rEnd; ++i) {
        for (int j = col_start; j < cEnd; ++j) {
            long long sum = 0;
            for (int p = 0; p < N; ++p) {
                sum += static_cast<long long>(A[i][p]) * B[p][j];
            }
            C[i][j] = sum;
        }
    }
}

void clearMatrix() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i][j] = 0;
        }
    }
}

int main() {
    std::cout << "=== Universal Matrix Multiplication " << N << "x" << N << " (std::thread) ===" << std::endl;

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // 1. Генерация случайных матриц
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A[i][j] = std::rand() % 10;
            B[i][j] = std::rand() % 10;
        }
    }

    auto single_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            long long sum = 0;
            for (int p = 0; p < N; ++p) {
                sum += static_cast<long long>(A[i][p]) * B[p][j];
            }
            C[i][j] = sum;
        }
    }
    auto single_end = std::chrono::high_resolution_clock::now();
    double single_time_ms = std::chrono::duration<double, std::milli>(
        single_end - single_start).count();

    std::cout << "Single-threaded time: " << single_time_ms << " ms" << std::endl;
    std::cout << std::endl;

    std::vector<int> block_sizes = {1, 2, 5, 10, 20, 25, 50, 100, 200, 500, N};

    std::cout << "Multi-threaded results:" << std::endl;
    std::cout << "k\tThreads\tTime(ms)\tSpeedup" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    for (int k : block_sizes) {
        clearMatrix();

        int blocks_per_dim = (N + k - 1) / k;
        int total_threads = blocks_per_dim * blocks_per_dim;

        if (total_threads > kMaxThreads) {
            std::cout << k << "\t" << total_threads << "\tSKIPPED\t\tTOO MANY THREADS" << std::endl;
            continue;
        }

        std::vector<std::thread> threads;
        threads.reserve(total_threads);

        auto start_time = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < N; i += k) {
            for (int j = 0; j < N; j += k) {
                threads.emplace_back(multiplyBlock, i, j, k);
            }
        }

        for (auto& thread : threads) {
            thread.join();
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        double multi_time_ms = std::chrono::duration<double, std::milli>(
            end_time - start_time).count();

        double speedup = single_time_ms / multi_time_ms;

        std::cout << k << "\t" << total_threads << "\t"
                  << std::fixed << std::setprecision(2) << multi_time_ms << "\t\t"
                  << std::setprecision(3) << speedup << "x" << std::endl;
    }

    std::cout << std::endl << "Universal implementation completed!" << std::endl;
    return 0;
}

/*

=== Universal Matrix Multiplication 1000x1000 (std::thread) ===
Single-threaded time: 1939.72 ms

Multi-threaded results:
k	Threads	Time(ms)	Speedup
----------------------------------------
1	1000000	SKIPPED		TOO MANY THREADS
2	250000	SKIPPED		TOO MANY THREADS
5	40000	SKIPPED		TOO MANY THREADS
10	10000	319.93		6.063x
20	2500	204.35		9.492x
25	1600	189.01		10.262x
50	400	176.36		10.999x
100	100	171.52		11.309x
200	25	180.54		10.744x
500	4	407.13		4.764x
1000	1	1625.77		1.193x

Universal implementation completed!

 */