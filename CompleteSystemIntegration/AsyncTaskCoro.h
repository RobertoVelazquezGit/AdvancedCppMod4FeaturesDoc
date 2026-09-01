#pragma once

#include <atomic>
#include <coroutine>
#include <cstddef>
#include <exception>
#include <functional>
#include <memory>
#include <queue>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

template<typename T>
class Task {
public:
    struct promise_type {
        T value;
        std::exception_ptr exception;

        Task get_return_object() {
            return Task{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }

        std::suspend_never initial_suspend() noexcept {
            return {};
        }

        std::suspend_always final_suspend() noexcept {
            return {};
        }

        void return_value(T val) {
            value = std::move(val);
        }

        void unhandled_exception() noexcept {
            exception = std::current_exception();
        }
    };

    using Handle = std::coroutine_handle<promise_type>;

    explicit Task(Handle handle) noexcept
        : coro(handle) {
    }

    ~Task() {
        if (coro) {
            coro.destroy();
        }
    }

    // Task is a move-only type because it has exclusive ownership of the coroutine frame.
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    Task(Task&& other) noexcept
        : coro(std::exchange(other.coro, Handle{})) {
    }

    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (coro) {
                coro.destroy();
            }

            coro = std::exchange(other.coro, Handle{});
        }

        return *this;
    }

    T get() {
        if (!coro || !coro.done()) {
            throw std::runtime_error("Task not completed");
        }

        if (coro.promise().exception) {
            std::rethrow_exception(coro.promise().exception);
        }

        // The coroutine is complete, so its promise contains the result.
        return coro.promise().value;
    }

    bool is_ready() const {
        return coro && coro.done();
    }

private:
    Handle coro;
};

class AsyncTaskManager {
private:
    std::queue<std::function<void()>> taskQueue;
    std::vector<std::thread> workers;
    std::atomic<bool> running{ true };

public:
    explicit AsyncTaskManager(
        std::size_t numWorkers = std::thread::hardware_concurrency());
    ~AsyncTaskManager();

    AsyncTaskManager(const AsyncTaskManager&) = delete;
    AsyncTaskManager& operator=(const AsyncTaskManager&) = delete;

    // Coroutine for simulating asynchronous work.
    Task<std::string> processDataAsync(int id, int processingTimeMs);

    // Coroutine for batch processing.
    Task<std::vector<std::string>> batchProcess(const std::vector<int>& dataIds);

private:
    void workerLoop();
};

// Run the asynchronous task coroutine test bench.
void runAsyncTaskCoro(std::shared_ptr<AsyncTaskManager> asyncTaskManager);
