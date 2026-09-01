// C++20 required for coroutines

#include <coroutine>
#include <iostream>

class Generator // USER-DEFINED NAME: can be changed.
{
public:

    // REQUIRED NAME.
    // The compiler looks specifically for a type named "promise_type".
    struct promise_type
    {
        // USER-DEFINED NAME.
        // This variable is only used by our implementation.
        int current_value{};

        // REQUIRED NAME.
        // The compiler calls get_return_object() when creating the coroutine.
        Generator get_return_object()
        {
            return Generator{
                std::coroutine_handle<promise_type>::from_promise(*this)
            };
        }

        // REQUIRED NAME.
        // The compiler calls initial_suspend() before executing
        // the body of the coroutine.
		// When coroutines are first created, they are suspended before executing the body of the coroutine.    
        std::suspend_always initial_suspend()
        {
            return {};
        }

        // REQUIRED NAME.
        // The compiler calls final_suspend() when the coroutine finishes.
        std::suspend_always final_suspend() noexcept
        {
            return {};
        }

        // REQUIRED NAME when using co_yield.
        // co_yield value causes the compiler to call yield_value(value).
        std::suspend_always yield_value(int value)
        {
            current_value = value;
            return {};
        }

        // REQUIRED NAME for a coroutine that finishes without
        // returning a value with co_return.
        void return_void()
        {
        }

        // REQUIRED NAME.
        // Called if an exception escapes from the coroutine.
        void unhandled_exception()
        {
            std::terminate();
        }
    };


    // USER-DEFINED NAME.
    // "Handle" is only an alias chosen by us.
    // std::coroutine_handle itself is a standard C++ type.
    using Handle = std::coroutine_handle<promise_type>;


    // USER-DEFINED constructor.
    explicit Generator(Handle handle)
        : handle_(handle)
    {
    }


    // USER-DEFINED.
    // We decide that our Generator cannot be copied.
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;


    // Destructor name depends on the class name, as usual in C++.
    ~Generator()
    {
        if (handle_)
        {
            handle_.destroy();
        }
    }


    // USER-DEFINED NAME.
    // "next" is NOT part of the C++ coroutine protocol.
    // We could call it resume(), advance(), etc.
    bool next()
    {
        if (!handle_ || handle_.done())
        {
            return false;
        }

        handle_.resume();

        return !handle_.done();
    }


    // USER-DEFINED NAME.
    // "value" is NOT required by the compiler.
    int value() const
    {
		return handle_.promise().current_value;  // current_value is a variable we defined in promise_type. 
    }


private:

    // USER-DEFINED NAME.
    // The variable could be called coroutine_, h_, myHandle_, etc.
    Handle handle_;
};


// USER-DEFINED NAME.
// "numbers" can be called anything.
//
// This function becomes a coroutine because it contains co_yield.
Generator numbers()
{
    std::cout << "Generating 10\n";
    co_yield 10;

    std::cout << "Generating 20\n";
    co_yield 20;

    std::cout << "Generating 30\n";
    co_yield 30;

    std::cout << "Generator finished\n";
}


int main()
{
    Generator generator = numbers();

    std::cout << "Generator created\n\n";

	// notice generator.next() return !handle_.done() is false when coroutine is finished,
    // so the loop is not entered and not access to  .value is tried, which would be undefined behavior. 
    while (generator.next())
    {
        std::cout << "Received: "
            << generator.value()
            << "\n\n";
    }

    return 0;
}