
#include "ConfigManager.h"
#include "ConceptsRanges.h"
#include "AsyncTaskCoro.h"

#include <coroutine>
#include <exception>
#include <iostream>
#include <memory>
#include <utility>

class TaskInteg {
public:
    // Required name: the compiler looks for TaskInteg::promise_type to manage the coroutine.
    struct promise_type;

    // Not required by the coroutine protocol; this alias makes the handle type easier to use.
    using Handle = std::coroutine_handle<promise_type>;

    struct promise_type {
        // Required: creates the object returned to the caller when the coroutine is invoked.
        TaskInteg get_return_object() {
            return TaskInteg{ Handle::from_promise(*this) };
        }

        // Required: controls whether execution starts immediately or is initially suspended.
        // suspend_never makes the coroutine start as soon as it is created.
        std::suspend_never initial_suspend() noexcept {
            return {};
        }

        // Required: controls what happens when the coroutine reaches its end.
        // suspend_always keeps the coroutine frame alive until its owning TaskInteg destroys it.
        std::suspend_always final_suspend() noexcept {
            return {};
        }

        // Required for a coroutine that uses co_return without returning a value.
        void return_void() noexcept {
        }

        // Required: handles exceptions that escape from the coroutine body.
        void unhandled_exception() noexcept {
            std::terminate();
        }
    };

    explicit TaskInteg(Handle handle) noexcept
        : handle_(handle) {
    }

    ~TaskInteg() {
        if (handle_) {
            handle_.destroy();
        }
    }

    TaskInteg(const TaskInteg&) = delete;
    TaskInteg& operator=(const TaskInteg&) = delete;

    TaskInteg(TaskInteg&& other) noexcept
        : handle_(std::exchange(other.handle_, Handle{})) {
    }

    TaskInteg& operator=(TaskInteg&& other) noexcept {
        if (this != &other) {
            if (handle_) {
                handle_.destroy();
            }

            handle_ = std::exchange(other.handle_, Handle{});
        }

        return *this;
    }

    // Resume the coroutine if it has not already completed.
    // Return true when the coroutine remains suspended and can be resumed again.
    bool resume() {
        if (!handle_ || handle_.done()) {
            return false;
        }

        handle_.resume();
        return !handle_.done();
    }

private:
    // Not required by the protocol; TaskInteg owns this handle so it can destroy the coroutine frame.
    Handle handle_;
};

TaskInteg runCompleteSystemIntegration()
{
    // Stage 1: Run the configuration management test bench.
    auto configManager = std::make_shared<ConfigManager>();
    runConfigManagerBenchmark(configManager);

    // Suspend the coroutine before starting the next stage.
    co_await std::suspend_always{};

    // Stage 2: Run the concepts and ranges test bench.
    auto dataProcessor = std::make_shared<DataProcessor>();
    runConceptsRanges(dataProcessor);

    // Suspend the coroutine before starting the next stage.
    co_await std::suspend_always{};

    // Stage 3: Run the asynchronous task coroutine test bench.
    auto asyncTaskManager = std::make_shared<AsyncTaskManager>();
    runAsyncTaskCoro(asyncTaskManager);
}

int main()
{
    auto task = runCompleteSystemIntegration();

    // Resume the coroutine to execute Stage 2 and suspend before Stage 3.
    // This returns true because the coroutine suspends again before Stage 3.
    bool stage2CanResume = task.resume();
    std::cout << "Stage 2 resume returned: "
        << std::boolalpha << stage2CanResume << '\n';

    // Resume the coroutine again to execute Stage 3.
    // This returns false because the coroutine completes after Stage 3.
    bool stage3CanResume = task.resume();
    std::cout << "Stage 3 resume returned: "
        << std::boolalpha << stage3CanResume << '\n';
}
