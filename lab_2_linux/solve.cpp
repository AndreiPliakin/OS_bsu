#include <pthread.h>

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <vector>

const int N = 1000;
constexpr int kMaxThreads = 10000;

static int A[N][N];
static int B[N][N];
static int64_t C[N][N];

struct __attribute__((aligned(64))) BlockParam {
    int row_start;
    int col_start;
    int size;
    int padding[13];
};

void* threadMultiply(void* arg) {
    auto* param = static_cast<BlockParam *>(arg);
    int r0 = param->row_start;
    int c0 = param->col_start;
    int blockSize = param->size;
    int rEnd = (r0 + blockSize < N) ? r0 + blockSize : N;
    int cEnd = (c0 + blockSize < N) ? c0 + blockSize : N;
    for (int i = r0; i < rEnd; ++i) {
        for (int j = c0; j < cEnd; ++j) {
            int64_t sum = 0;
            for (int p = 0; p < N; ++p) {
                sum += static_cast<int64_t>(A[i][p]) * B[p][j];
            }
            C[i][j] = sum;
        }
    }
    pthread_exit(nullptr);
}
int main() {
    std::srand(std::time(nullptr));

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A[i][j] = std::rand() % 10;
            B[i][j] = std::rand() % 10;
        }
    }
    auto single_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int64_t sum = 0;
            for (int p = 0; p < N; ++p) {
                sum += static_cast<int64_t>(A[i][p]) * B[p][j];
            }
            C[i][j] = sum;
        }
    }
    auto single_end = std::chrono::high_resolution_clock::now();
    double single_time_ms =
        std::chrono::duration<double, std::milli>(single_end - single_start)
            .count();
    std::cout << "Однопоточное умножение: " << single_time_ms << " мс"
              << std::endl;

    std::cout << "Multi-threaded results:" << std::endl;
    std::cout << "k\tThreads\tTime(ms)\tSpeedup" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    std::vector ks = {1, 2, 5, 10, 20, 25, 50, 100, 200, 500, N};
    for (int k : ks) {
        int blocks_per_dim = (N + k - 1) / k;
        int total_threads = blocks_per_dim * blocks_per_dim;

        if (total_threads > kMaxThreads) {
            std::cout << k << "\t" << total_threads << "\tSKIPPED\t\tTOO MANY THREADS" << std::endl;
            continue;
        }

        std::vector<pthread_t> threads(total_threads);
        std::vector<BlockParam> params(total_threads);
        params.reserve(total_threads);

        auto start_time = std::chrono::high_resolution_clock::now();
        int t = 0;
        for (int i = 0; i < N; i += k) {
            for (int j = 0; j < N; j += k) {
            params[t] = {i, j, k, {0}};
                if (pthread_create(&threads[t], nullptr, threadMultiply,
                                   &params[t]) != 0) {
                    std::cerr << "Ошибка при создании потока\n";
                }
                ++t;
            }
        }
        for (int i = 0; i < total_threads; ++i) {
            pthread_join(threads[i], nullptr);
        }
        auto end_time = std::chrono::high_resolution_clock::now();

        double multi_time_ms = std::chrono::duration<double, std::milli>(
            end_time - start_time).count();

        double speedup = single_time_ms / multi_time_ms;

        std::cout << k << "\t" << total_threads << "\t"
                  << std::fixed << std::setprecision(2) << multi_time_ms << "\t\t"
                  << std::setprecision(3) << speedup << "x" << std::endl;
    }
    return 0;
}

/*

Однопоточное умножение: 1917.26 мс
Multi-threaded results:
k	Threads	Time(ms)	Speedup
----------------------------------------
1	1000000	SKIPPED		TOO MANY THREADS
2	250000	SKIPPED		TOO MANY THREADS
5	40000	SKIPPED		TOO MANY THREADS
10	10000	331.19		5.789x
20	2500	205.23		9.342x
25	1600	186.68		10.270x
50	400	168.53		11.376x
100	100	167.29		11.460x
200	25	173.72		11.036x
500	4	396.59		4.834x
1000	1	1514.30		1.266x


 */