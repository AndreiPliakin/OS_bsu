#define NOMINMAX 
#include <iostream>
#include <vector>
#include <windows.h>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <algorithm>

const int OPTIMAL_THREADS = 4;
const int N = 100;

static int A[N][N];
static int B[N][N];
static long long C[N][N];

struct BlockParam {
    int row_start;
    int col_start;
    int size;
};

DWORD WINAPI ThreadMultiply(LPVOID lpParam) {
    BlockParam* param = (BlockParam*)lpParam;
    int r0 = param->row_start;
    int c0 = param->col_start;
    int blockSize = param->size;

    int rEnd = std::min(r0 + blockSize, N);
    int cEnd = std::min(c0 + blockSize, N);

    for (int i = r0; i < rEnd; ++i) {
        for (int j = c0; j < cEnd; ++j) {
            long long sum = 0;
            for (int p = 0; p < N; ++p) {
                sum += static_cast<long long>(A[i][p]) * B[p][j];
            }
            C[i][j] = sum;
        }
    }
    return 0;
}

void clearMatrix() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i][j] = 0;
        }
    }
}

int main() {
    std::cout << "Windows API (CreateThread) - Matrix Multiplication " << N << "x" << N << std::endl;
    std::cout << "==================================================" << std::endl;

    std::srand(static_cast<unsigned>(std::time(nullptr)));

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
    std::cout << "Single-threaded: " << single_time_ms << " ms" << std::endl;
    std::cout << std::endl;

    int ks[] = { 1, 2, 5, 10, 20, 50, N };

    for (int k : ks) {
        clearMatrix();

        int blocks_per_dim = (N + k - 1) / k;
        int total_threads = blocks_per_dim * blocks_per_dim;

        std::vector<HANDLE> threads;
        threads.reserve(total_threads);
        std::vector<BlockParam> params;
        params.reserve(total_threads);

        auto start_time = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < N; i += k) {
            for (int j = 0; j < N; j += k) {
                params.push_back({ i, j, k });

                HANDLE hThread = CreateThread(
                    NULL,                 
                    0,                    
                    ThreadMultiply,        
                    &params.back(),        
                    0,                  
                    NULL                
                );

                if (hThread == NULL) {
                    std::cerr << "CreateThread failed! Error: " << GetLastError() << std::endl;
                    return 1;
                }
                threads.push_back(hThread);
            }
        }

        for (HANDLE hThread : threads) {
            WaitForSingleObject(hThread, INFINITE);
            CloseHandle(hThread);
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        double multi_time_ms = std::chrono::duration<double, std::milli>(
            end_time - start_time).count();

        double speedup = single_time_ms / multi_time_ms;

        std::cout << "Block size k = " << k
            << " | Threads = " << total_threads
            << " | Time = " << multi_time_ms << " ms"
            << " | Speedup = " << speedup << "x"
            << std::endl;
    }

    std::cout << std::endl << "Windows API implementation completed!" << std::endl;
    return 0;
}