#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>

// Shared resources
std::queue<int> taskQueue;
std::mutex queueMutex;
std::condition_variable conditionVar;
bool stopProcessing = false;

// Producer function
void producer(int id, int numTasks) {
    for (int i = 0; i < numTasks; ++i) {
        // Simulate work for producing a task
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Lock the mutex before modifying the shared queue
        std::lock_guard<std::mutex> lock(queueMutex);
        taskQueue.push(i);
        std::cout << "Producer " << id << " produced task " << i << std::endl;

        // Notify a consumer that a task is available
        conditionVar.notify_one();
    }
}

// Consumer function
void consumer(int id) {
    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex);

        // Wait until a task is available or stopProcessing is set
        conditionVar.wait(lock, [] { return !taskQueue.empty() || stopProcessing; });

        // Stop if no more tasks and processing is complete
        if (stopProcessing && taskQueue.empty()) break;

        // Process a task
        int task = taskQueue.front();
        taskQueue.pop();
        lock.unlock(); // Unlock mutex while processing the task

        std::cout << "Consumer " << id << " processed task " << task << std::endl;

        // Simulate task processing time
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}

int main() {
    const int numProducers = 2;
    const int numConsumers = 3;
    const int tasksPerProducer = 5;

    // Create producer threads
    std::vector<std::thread> producers;
    for (int i = 0; i < numProducers; ++i) {
        producers.emplace_back(producer, i, tasksPerProducer);
    }

    // Create consumer threads
    std::vector<std::thread> consumers;
    for (int i = 0; i < numConsumers; ++i) {
        consumers.emplace_back(consumer, i);
    }

    // Wait for producers to finish
    for (auto& producer : producers) {
        producer.join();
    }

    // Notify consumers to stop processing
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stopProcessing = true;
    }
    conditionVar.notify_all();

    // Wait for consumers to finish
    for (auto& consumer : consumers) {
        consumer.join();
    }

    return 0;
}


/*

**“In this producer-consumer implementation,
 I used std::mutex to synchronize access to the shared task queue and std::condition_variable to 
 notify threads when a task is added or when processing is complete.

Producers generate tasks and push them into the queue while holding 
a std::lock_guard to ensure thread safety. After adding a task, they notify one waiting consumer using conditionVar.notify_one().

Consumers continuously check for tasks using a std::unique_lock and wait
 on the condition_variable until tasks are available. 
 The wait function prevents spurious wakeups by using a lambda condition that ensures either tasks are present
  or processing is flagged to stop. Once a task is retrieved, the consumer unlocks the mutex to process the task, 
  ensuring minimal blocking for other threads.

To gracefully shut down, I used a flag stopProcessing that, when set, 
allows consumers to exit their loops after completing the remaining tasks in the queue. 
This design avoids race conditions, ensures all tasks are processed, and allows for clean termination.”**



*/