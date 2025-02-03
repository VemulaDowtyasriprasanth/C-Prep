#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

template <typename T>
class ThreadSafeQueue {
private:
    std::queue<T> queue;               // Underlying queue
    mutable std::mutex queueMutex;     // Mutex to protect shared resource
    std::condition_variable condVar;   // Condition variable for synchronization

public:
    ThreadSafeQueue() = default;

    // Add an item to the queue
    void enqueue(const T& item) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            queue.push(item);
        }
        condVar.notify_one(); // Notify a waiting thread
    }

    // Retrieve and remove an item from the queue (blocks if empty)
    T dequeue() {
        std::unique_lock<std::mutex> lock(queueMutex);
        condVar.wait(lock, [this] { return !queue.empty(); }); // Wait until the queue is not empty

        T item = queue.front();
        queue.pop();
        return item;
    }

    // Try to dequeue an item without blocking (returns std::optional)
    std::optional<T> tryDequeue() {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (queue.empty()) {
            return std::nullopt; // Return empty optional if queue is empty
        }

        T item = queue.front();
        queue.pop();
        return item;
    }

    // Check if the queue is empty
    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(queueMutex);
        return queue.empty();
    }

    // Get the current size of the queue
    size_t size() const {
        std::lock_guard<std::mutex> lock(queueMutex);
        return queue.size();
    }
};

// Example usage
int main() {
    ThreadSafeQueue<int> tsQueue;

    // Producer thread
    std::thread producer([&]() {
        for (int i = 0; i < 10; ++i) {
            tsQueue.enqueue(i);
            std::cout << "Produced: " << i << std::endl;
        }
    });

    // Consumer thread
    std::thread consumer([&]() {
        for (int i = 0; i < 10; ++i) {
            int item = tsQueue.dequeue();
            std::cout << "Consumed: " << item << std::endl;
        }
    });

    producer.join();
    consumer.join();

    return 0;
}



/*

Key Features
Thread Safety with std::mutex:

A std::mutex ensures that only one thread accesses the queue at a time, preventing data races during enqueue and dequeue operations.
Synchronization with std::condition_variable:

Consumers waiting to dequeue are notified when a producer enqueues an item using notify_one().
This avoids busy waiting and ensures efficient resource usage.
Non-blocking Dequeue (tryDequeue):

The tryDequeue method allows consumers to attempt retrieval without blocking, useful in scenarios where blocking is undesirable.
Minimal Lock Contention:

Locks are scoped (RAII with std::lock_guard or std::unique_lock) to minimize their duration and ensure other threads aren’t unnecessarily delayed.
No Deadlocks:

By carefully locking only what’s necessary and avoiding nested locks or shared locks across threads, the design guarantees deadlock-free operation.
Explanation (Interview-Style)
**"This implementation of a thread-safe queue is designed to handle concurrent access without risking data races or deadlocks.

Producers and consumers operate on the queue while protected by a std::mutex. To ensure minimal lock contention, locks are scoped to only protect critical sections—such as when adding or removing items from the queue. A std::condition_variable is used for synchronization, allowing consumers to block efficiently while waiting for items to be added. This ensures resource efficiency and avoids busy-waiting.

The tryDequeue method is a non-blocking alternative for consumers who prefer not to wait. Additionally, careful design avoids deadlocks entirely by ensuring no nested locks are used. In a production environment, this implementation could be extended with better logging and monitoring tools to debug contention or queue size issues under load."**






*/