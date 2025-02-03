#include <iostream>
#include <vector>
#include <thread>
#include <numeric>  // for std::accumulate

long long parallelSum(const std::vector<int>& data, size_t numThreads) {
    // 1. Calculate chunk size
    size_t dataSize = data.size();
    size_t chunkSize = (dataSize + numThreads - 1) / numThreads;  // ceiling division

    // 2. Vector to store partial sums from each thread
    std::vector<long long> partialSums(numThreads, 0);

    // 3. Lambda function for each thread to sum its chunk
    auto worker = [&](size_t threadID) {
        // Determine the start and end indices for this thread
        size_t start = threadID * chunkSize;
        size_t end   = std::min(start + chunkSize, dataSize);

        // Compute local sum
        long long localSum = 0;
        for (size_t i = start; i < end; ++i) {
            localSum += data[i];
        }

        // Store the partial sum in the shared vector
        partialSums[threadID] = localSum;
    };

    // 4. Create and launch threads
    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    for (size_t i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker, i);
    }

    // 5. Join threads
    for (auto& t : threads) {
        t.join();
    }

    // 6. Sum all partial sums
    long long totalSum = 0;
    for (auto& val : partialSums) {
        totalSum += val;
    }

    return totalSum;
}

// Example usage:
int main() {
    std::vector<int> data(1000000, 1); // Large vector of 1's
    size_t numThreads = 4;
    long long result = parallelSum(data, numThreads);

    std::cout << "Sum = " << result << std::endl;
    return 0;
}


/*

**“To implement a parallel sum for a large vector, I typically split the data into chunks—one chunk per
 thread—so that each thread works independently on its subset of elements. This means each thread calculates 
 a local (partial) sum without needing to write to a shared variable. By structuring the computation this way,
  I avoid race conditions because there’s no concurrent write operation to the same memory location.

In the code example, I use a partialSums array where each thread stores its own result. After the threads 
finish, I join them and aggregate their partial sums in the main thread. This design keeps concurrency simple and efficient.

If I needed to share a single sum variable, I’d protect it with a mutex or use atomic operations. 
However, splitting the work into separate sums is typically more performant and easier to reason about. 
For large-scale production environments, I’d also consider a proper thread pool or parallel algorithms
 (like C++17’s std::reduce with execution policies) to manage resources more elegantly.”**




*/