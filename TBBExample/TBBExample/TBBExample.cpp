#include <iostream>
#include <vector>
#include <chrono>
#include <tbb/tbb.h>
#include <atomic>

void sum_without_tbb(const std::vector<int>& vec) {
    long long sum = 0;
    for (size_t i = 0; i < vec.size(); ++i) {
        sum += vec[i];
    }
}

void sum_with_tbb(const std::vector<int>& vec) {
    long long sum = 0;

    std::atomic<long long> atomic_sum(0);

    tbb::parallel_for(tbb::blocked_range<size_t>(0, vec.size()),
        [&vec, &atomic_sum](const tbb::blocked_range<size_t>& r) {
            long long local_sum = 0;
            for (size_t i = r.begin(); i != r.end(); ++i) {
                local_sum += vec[i];
            }
            atomic_sum.fetch_add(local_sum, std::memory_order_relaxed);
        });

    sum = atomic_sum.load(std::memory_order_relaxed);
}

int main() {
    const size_t N = 10000000;
    std::vector<int> vec(N, 1);

    auto start = std::chrono::high_resolution_clock::now();
    sum_without_tbb(vec);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time without TBB: " << duration.count() << " seconds." << std::endl;

    start = std::chrono::high_resolution_clock::now();
    sum_with_tbb(vec);
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Time with TBB: " << duration.count() << " seconds." << std::endl;

    return 0;
}
