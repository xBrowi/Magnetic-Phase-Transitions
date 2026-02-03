#include <thread>
#include <iostream>
#include <vector>
#include <chrono>
#include <mutex>

void threadFunction(int threadID, std::mutex& mtx) {

    {
        // Lock mutex for safe console output
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Thread " << threadID << " is starting." << std::endl;
    }

    // Simulate some work with sleep
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    // Lock mutex for safe console output
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Thread " << threadID << " finished." << std::endl;
    }
}

int main() {
    // use all available hardware threads
    const int numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    std::mutex mtx;

    // Create and launch threads
    for (int i = 0; i < numThreads; ++i) {
        threadFunction(i + 1, mtx);
        //threads.emplace_back(threadFunction, i + 1, std::ref(mtx));
    }

    // Join threads to the main thread
    for (auto& th : threads) {
        th.join();
    }

    std::cout << "All threads have completed execution." << std::endl;
    return 0;
}