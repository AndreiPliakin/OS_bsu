#include <pthread.h>

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
const int N = 100;
static int A[N][N];
static int B[N][N];
static long long C[N][N];
struct BlockParam {
    int row_start;
    int col_start;
    int size;
};

void* threadMultiply(void* arg) {
    auto* param = (BlockParam*)arg;
    int r0 = param->row_start;
    int c0 = param->col_start;
    int blockSize = param->size;
    int rEnd = (r0 + blockSize < N) ? r0 + blockSize : N;
    int cEnd = (c0 + blockSize < N) ? c0 + blockSize : N;
    for (int i = r0; i < rEnd; ++i) {
        for (int j = c0; j < cEnd; ++j) {
            C[i][j] = 0;
            for (int p = 0; p < N; ++p) {
                C[i][j] += (long long)A[i][p] * B[p][j];
            }
        }
    }
    pthread_exit(NULL);
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
            long long sum = 0;
            for (int p = 0; p < N; ++p) {
                sum += (long long)A[i][p] * B[p][j];
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
    int ks[] = {1, 2, 5, 10, 20, 50, N};
    for (int k : ks) {
        int blocks_per_dim = (N + k - 1) / k;
        int total_threads = blocks_per_dim * blocks_per_dim;
        std::vector<pthread_t> threads(total_threads);
        std::vector<BlockParam> params;
        params.reserve(total_threads);
        auto start_time = std::chrono::high_resolution_clock::now();
        int t = 0;
        for (int i = 0; i < N; i += k) {
            for (int j = 0; j < N; j += k) {
                BlockParam param = {i, j, k};
                params.push_back(param);
                if (pthread_create(&threads[t], NULL, threadMultiply,
                                   &params[t]) != 0) {
                    std::cerr << "Ошибка при создании потока\n";
                }
                ++t;
            }
        }
        for (int t = 0; t < total_threads; ++t) {
            pthread_join(threads[t], NULL);
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        double multi_time_ms =
            std::chrono::duration<double, std::milli>(end_time - start_time)
                .count();
        std::cout << "k = " << k << ", число потоков = " << total_threads
                  << ", время многопоточно (pthread): " << multi_time_ms
                  << " мс" << std::endl;
    }
    return 0;
}