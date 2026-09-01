#include "AsyncTaskCoro.h"

#include <chrono>
#include <iostream>

AsyncTaskManager::AsyncTaskManager(std::size_t numWorkers) {
    for (std::size_t i = 0; i < numWorkers; ++i) {
        workers.emplace_back([this] { workerLoop(); });
    }
}

AsyncTaskManager::~AsyncTaskManager() {
    running = false;

    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

Task<std::string> AsyncTaskManager::processDataAsync(
    int id,
    int processingTimeMs) {
    // Simulate asynchronous processing. This blocks the current thread.
    std::this_thread::sleep_for(std::chrono::milliseconds(processingTimeMs));

    // Complex processing logic would be placed here.
    std::string result = "Processed data " + std::to_string(id) +
        " in " + std::to_string(processingTimeMs) + "ms";

    // This is a coroutine, so use co_return to return the result.
    co_return result;
}

Task<std::vector<std::string>> AsyncTaskManager::batchProcess(
    const std::vector<int>& dataIds) {
    std::vector<Task<std::string>> tasks;

    // Start all tasks. This is not true parallelism because initial_suspend()
    // starts each coroutine immediately and sleep_for() blocks the caller.
    for (int id : dataIds) {
        tasks.push_back(processDataAsync(id, 100 + (id % 500)));
    }

    // Collect results.
    std::vector<std::string> results;
    for (auto& task : tasks) {
        while (!task.is_ready()) {
            // This would require an external scheduler to resume batchProcess.
            // In this example, every inner task has already completed.
            co_await std::suspend_always{};
        }

        results.push_back(task.get());
    }

    // This is a coroutine, so use co_return to return the results.
    co_return results;
}

void AsyncTaskManager::workerLoop() {
    // Worker implementation for task queue processing.
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // Process queued tasks.
    }
}

void runAsyncTaskCoro(std::shared_ptr<AsyncTaskManager> asyncTaskManager) {
    // There is no asynchronous threading in these coroutine calls. They run
    // on the main thread and block it until their work has finished.
    std::cout << "=== Testing processDataAsync ===\n";

    // processDataAsync starts immediately because initial_suspend()
    // returns std::suspend_never.
    auto task = asyncTaskManager->processDataAsync(1, 500);

    // The coroutine has already finished because sleep_for() blocks the
    // current thread until processing completes.
    if (task.is_ready()) {
        std::cout << "Task completed\n";
        std::cout << task.get() << '\n';
    }

    std::cout << "\n=== Testing batchProcess ===\n";

    std::vector<int> ids{ 1, 2, 3, 4, 5 };

    // batchProcess starts immediately. processDataAsync() also starts
    // immediately for every ID, so all inner tasks have completed when
    // batchProcess reaches its result collection loop.
    auto batchTask = asyncTaskManager->batchProcess(ids);

    if (batchTask.is_ready()) {
        std::cout << "Batch completed\n";

        auto results = batchTask.get();

        for (const auto& result : results) {
            std::cout << result << '\n';
        }
    }
}
