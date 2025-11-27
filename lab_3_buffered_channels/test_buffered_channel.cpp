#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include "buffered_channel.h"

const int N = 1000;
constexpr int kMaxThreads = 10000;

static int A[N][N];
static int B[N][N];
static int64_t C[N][N];


struct BlockTask {
    int row_start;
    int col_start;
    int block_size;
};

struct BlockResult {
    int row_start;
    int col_start;
    int block_size;
    int64_t block_sum;
};


void multiplyBlock(const BlockTask& task, std::vector<int64_t>& local_results) {
    int rEnd = std::min(task.row_start + task.block_size, N);
    int cEnd = std::min(task.col_start + task.block_size, N);

    for (int i = task.row_start; i < rEnd; ++i) {
        for (int j = task.col_start; j < cEnd; ++j) {
            int64_t sum = 0;
            for (int p = 0; p < N; ++p) {
                sum += static_cast<int64_t>(A[i][p]) * B[p][j];
            }
            C[i][j] = sum;
            local_results.push_back(sum);
        }
    }
}


void worker_thread(buffered_channel<BlockTask>& task_channel,
                   buffered_channel<BlockResult>& result_channel,
                   std::atomic<int>& active_workers) {
    active_workers++;

    try {
        while (true) {
            auto [task, valid] = task_channel.recv();
            if (!valid) {
                break;
            }

            std::vector<int64_t> local_results;
            multiplyBlock(task, local_results);

            int64_t block_sum = 0;
            for (auto val : local_results) {
                block_sum += val;
            }

            BlockResult result{task.row_start, task.col_start, task.block_size, block_sum};
            result_channel.send(result);
        }
    } catch (const std::exception& e) {
        std::cerr << "Worker thread error: " << e.what() << std::endl;
    }

    active_workers--;
}

void clearMatrix() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            C[i][j] = 0;
        }
    }
}

int main() {
    std::cout << "=== Matrix Multiplication with Buffered Channel " << N << "x" << N << " ===" << std::endl;

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
            int64_t sum = 0;
            for (int p = 0; p < N; ++p) {
                sum += static_cast<int64_t>(A[i][p]) * B[p][j];
            }
            C[i][j] = sum;
        }
    }
    auto single_end = std::chrono::high_resolution_clock::now();
    double single_time_ms = std::chrono::duration<double, std::milli>(
        single_end - single_start).count();

    std::cout << "Single-threaded time: " << single_time_ms << " ms" << std::endl;
    std::cout << std::endl;

    std::vector<int> block_sizes = {10, 20, 25, 50, 100, 200, 500, N};
    const int num_worker_threads = std::thread::hardware_concurrency();

    std::cout << "Using " << num_worker_threads << " worker threads" << std::endl;
    std::cout << "Multi-threaded results with channels:" << std::endl;
    std::cout << "k\tThreads\tTime(ms)\tSpeedup" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    for (int k : block_sizes) {
        clearMatrix();

        int blocks_per_dim = (N + k - 1) / k;
        int total_tasks = blocks_per_dim * blocks_per_dim;

        if (total_tasks > kMaxThreads) {
            std::cout << k << "\t" << total_tasks << "\tSKIPPED\t\tTOO MANY TASKS" << std::endl;
            continue;
        }

        buffered_channel<BlockTask> task_channel(total_tasks);
        buffered_channel<BlockResult> result_channel(total_tasks);

        std::atomic<int> active_workers{0};
        std::vector<std::thread> workers;

        auto start_time = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < num_worker_threads; ++i) {
            workers.emplace_back(worker_thread,
                               std::ref(task_channel),
                               std::ref(result_channel),
                               std::ref(active_workers));
        }

        std::thread generator([&task_channel, k, total_tasks]() {
            try {
                for (int i = 0; i < N; i += k) {
                    for (int j = 0; j < N; j += k) {
                        BlockTask task{i, j, k};
                        task_channel.send(task);
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Generator error: " << e.what() << std::endl;
            }
            task_channel.close();
        });


        std::thread collector([&result_channel, total_tasks]() {
            int results_received = 0;
            try {
                while (results_received < total_tasks) {
                    auto [result, valid] = result_channel.recv();
                    if (!valid) {
                        break;
                    }
                    results_received++;

                }
            } catch (const std::exception& e) {
                std::cerr << "Collector error: " << e.what() << std::endl;
            }
            result_channel.close();
        });

        generator.join();
        collector.join();

        while (active_workers > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        double multi_time_ms = std::chrono::duration<double, std::milli>(
            end_time - start_time).count();

        double speedup = single_time_ms / multi_time_ms;

        std::cout << k << "\t" << total_tasks << "\t"
                  << std::fixed << std::setprecision(2) << multi_time_ms << "\t\t"
                  << std::setprecision(3) << speedup << "x" << std::endl;
    }

    std::cout << std::endl << "Channel-based implementation completed!" << std::endl;
    return 0;
}

/*

=== Matrix Multiplication with Buffered Channel 1000x1000 ===
Single-threaded time: 1967.72 ms

Using 16 worker threads
Multi-threaded results with channels:
k	Threads	Time(ms)	Speedup
----------------------------------------
10	10000	174.52		11.275x
20	2500	171.60		11.467x
25	1600	173.76		11.324x
50	400	172.52		11.406x
100	100	177.13		11.109x
200	25	199.85		9.846x
500	4	428.66		4.590x
1000	1	1710.28		1.151x


 */