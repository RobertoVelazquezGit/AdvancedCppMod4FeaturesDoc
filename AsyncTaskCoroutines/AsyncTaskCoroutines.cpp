/*
Build an advanced task management system using C++20 coroutines for handling asynchronous operations elegantly.
 Practice
In the code below, implement error handling and cancellation mechanisms for coroutines.
Create a scheduler that manages multiple coroutines with different priorities.
Build a complete async pipeline that processes data through multiple stages.
Integrate the coroutine system with the previous configuration manager and data processor.
*/

#include <coroutine>
#include <future>
#include <thread>
#include <chrono>
#include <iostream>
#include <queue>
#include <optional>
#include <utility>

template<typename T>
class Task {
public:
    struct promise_type {
        T value;
        std::exception_ptr exception;

        Task get_return_object() {
            return Task{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }

        std::suspend_never initial_suspend() { return {}; }
        // suspend_never here means a different behavior of coro.done()
        // with suspend_always do not enter into while (!task.is_ready()) { at batchProcess,
        // i.e., bool is_ready() const { return coro.done(); } return true when coroutine finished
        // Notice once the coroutine finished, 
        std::suspend_always final_suspend() noexcept  
        {
            return {};
        }

        void return_value(T val) {
            value = std::move(val);
        }

        void unhandled_exception() {
            exception = std::current_exception();
        }
    };

    std::coroutine_handle<promise_type> coro;

    Task(std::coroutine_handle<promise_type> h) : coro(h) {}
    ~Task() { if (coro) coro.destroy(); }

    // Move-only type
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    Task(Task&& other) noexcept : coro(std::exchange(other.coro, {})) {}
    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (coro) coro.destroy();
            coro = std::exchange(other.coro, {});
        }
        return *this;
    }

    T get() {
        if (!coro.done()) {
            throw std::runtime_error("Task not completed");
        }
        if (coro.promise().exception) {
            std::rethrow_exception(coro.promise().exception);
        }
        return coro.promise().value;
    }

    bool is_ready() const { return coro.done(); }
};

class AsyncTaskManager {
private:
    std::queue<std::function<void()>> taskQueue;
    std::vector<std::thread> workers;
    std::atomic<bool> running{ true };

public:
    AsyncTaskManager(size_t numWorkers = std::thread::hardware_concurrency()) {
        for (size_t i = 0; i < numWorkers; ++i) {
            workers.emplace_back([this] { workerLoop(); });
        }
    }

    ~AsyncTaskManager() {
        running = false;
        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    // Coroutine for simulating async work
    Task<std::string> processDataAsync(int id, int processingTimeMs) {
        // Simulate async processing
        std::this_thread::sleep_for(std::chrono::milliseconds(processingTimeMs));

        // Complex processing logic here
        std::string result = "Processed data " + std::to_string(id) +
            " in " + std::to_string(processingTimeMs) + "ms";

		co_return result;  // this function is a coroutine, so we use co_return to return the result    
    }

	// Coroutine for batch processing   
    Task<std::vector<std::string>> batchProcess(const std::vector<int>& dataIds) {
        std::vector<Task<std::string>> tasks;

		// Start all tasks concurrently. This is not true parallelism, but simulates concurrent execution in a coroutine context.   
        for (int id : dataIds) {
            // Here the processDataAsync coroutine is created and started, but not awaited yet.
            // The actual processing will happen when we await the result.     
			tasks.push_back(processDataAsync(id, 100 + (id % 500)));  
        }

        // Collect results
        std::vector<std::string> results;
        for (auto& task : tasks) {
            while (!task.is_ready()) {
                // Here the batchProcess is suspended forever, but this never happens in this code because in the
                // tasks.push_back(processDataAsync(id, 100 + (id % 500))); the coroutine is started and terminated.
				// In fact would be suspended until someone does resume on the coroutine, but since we don't have a scheduler here, this will never happen. 
                co_await std::suspend_always{};  
            }
            results.push_back(task.get());
        }

		co_return results;  // this function is a coroutine, so we use co_return to return the results   
    }

private:
    void workerLoop() {
        // Worker implementation for task queue processing
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            // Process queued tasks
        }
    }
};

int main()
{
    AsyncTaskManager manager;

    std::cout << "=== Testing processDataAsync ===\n";

    // processDataAsync starts immediately because
    // initial_suspend() returns std::suspend_never.
    auto task = manager.processDataAsync(1, 500);

    // At this point the coroutine has already finished because
    // sleep_for() blocks the current thread until processing completes.
    if (task.is_ready())
    {
        std::cout << "Task completed\n";
        std::cout << task.get() << '\n';
    }


    std::cout << "\n=== Testing batchProcess ===\n";

    std::vector<int> ids{ 1, 2, 3, 4, 5 };

    // batchProcess starts immediately.
    //
    // processDataAsync() also starts immediately for every ID and
    // blocks until it finishes. Therefore, all inner tasks are already
    // completed when batchProcess reaches the result collection loop.
    auto batchTask = manager.batchProcess(ids);

    if (batchTask.is_ready())
    {
        std::cout << "Batch completed\n";

        auto results = batchTask.get();

        for (const auto& result : results)
        {
            std::cout << result << '\n';
        }
    }

    return 0;
}
