
#include "ConfigManager.h"
#include "ConceptsRanges.h"

#include <coroutine>
#include <exception>
#include <utility>

class Task {
public:
    // Required name: the compiler looks for Task::promise_type to manage the coroutine.
    struct promise_type;

    // Not required by the coroutine protocol; this alias makes the handle type easier to use.
    using Handle = std::coroutine_handle<promise_type>;

    struct promise_type {
        // Required: creates the object returned to the caller when the coroutine is invoked.
        Task get_return_object() {
            return Task{ Handle::from_promise(*this) };
        }

        // Required: controls whether execution starts immediately or is initially suspended.
        // suspend_never makes the coroutine start as soon as it is created.
        std::suspend_never initial_suspend() noexcept {
            return {};
        }

        // Required: controls what happens when the coroutine reaches its end.
        // suspend_always keeps the coroutine frame alive until its owning Task destroys it.
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

    explicit Task(Handle handle) noexcept
        : handle_(handle) {
    }

    ~Task() {
        if (handle_) {
            handle_.destroy();
        }
    }

    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    Task(Task&& other) noexcept
        : handle_(std::exchange(other.handle_, Handle{})) {
    }

    Task& operator=(Task&& other) noexcept {
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
    // Not required by the protocol; Task owns this handle so it can destroy the coroutine frame.
    Handle handle_;
};

Task runCompleteSystemIntegration()
{
    // Stage 1: Run the configuration management test bench.
    runConfigManagerBenchmark();

    // Suspend the coroutine before starting the next stage.
    co_await std::suspend_always{};

    // Stage 2: Run the concepts and ranges test bench.
    runConceptsRanges();
}

int main()
{
    auto task = runCompleteSystemIntegration();

    // Resume the coroutine to execute Stage 2.
    task.resume();
}
