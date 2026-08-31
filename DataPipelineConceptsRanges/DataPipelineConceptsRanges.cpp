/*
Create a high - performance data processing system using C++20 concepts and ranges for functional - style transformations.
 Practice
In the code below, implement additional concepts for string processing and custom data types.
Create complex transformation pipelines using ranges views composition.
Test the concepts by trying to pass invalid types and observe compilation errors.
Build a complete data analysis workflow that processes multiple data sets concurrently.
*/

#include <iostream>
#include <concepts>
#include <ranges>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>

// Define concepts for type constraints
template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

// Concept that requires a type to support < and > operators,
// with results convertible to bool.
template<typename T>
concept Comparable = requires(T a, T b) {
	{ a < b } -> std::convertible_to<bool>;  // this is one of the requirements for a type to be considered Comparable  
    { a > b } -> std::convertible_to<bool>;
};

template<typename Container>
concept NumericContainer = requires(Container c) {
    typename Container::value_type;  // this is one of the requirements for a type to be considered Comparable  
    requires Numeric<typename Container::value_type>;
    { c.begin() } -> std::input_iterator;
    { c.end() } -> std::input_iterator;
};

// Additional concept for string processing
template<typename T>
concept StringLike = std::convertible_to<T, std::string>;

// ToDo

class DataProcessor {
public:
    // Process numeric data with concepts-constrained templates
    template<NumericContainer auto container>
    auto analyzeData(decltype (container)& data) {
        using ValueType = typename std::remove_cvref_t<decltype(container)>::value_type;

        // Use ranges for functional-style processing
        auto positiveValues = data
            | std::views::filter([](const auto& x) { return x > 0; });

        auto squares = positiveValues
            | std::views::transform([](const auto& x) { return x * x; });

        // Calculate statistics
        ValueType sum = std::accumulate(squares.begin(), squares.end(), ValueType{ 0 });
        size_t count = std::distance(squares.begin(), squares.end());

        return std::make_tuple(sum, count, count > 0 ? sum / count : 0);
    }

    // Concepts-constrained sorting function
    template<Comparable T>
    void advancedSort(std::vector<T>& data, bool ascending = true) {
        if (ascending) {
            std::ranges::sort(data);
        }
        else {
            std::ranges::sort(data, std::greater<T>{});
        }
    }

	// Original
    // Generic filter with concepts
    //template<NumericContainer auto container, typename Predicate>
    //auto filterData(decltype (container)& data, Predicate pred)
    //    requires std::predicate<Predicate, typename std::remove_cvref_t<decltype(container)>::value_type>
    //{
    //    return data | std::views::filter(pred) | std::ranges::to<std::vector>();  // to vector is from c++23
    //}
    // Generic filter with concepts
    template<NumericContainer Container, typename Predicate>
    auto filterData(Container& data, Predicate pred)
        requires std::predicate<Predicate, typename Container::value_type>
    {
        auto filtered = data
            | std::views::filter(pred);

        using ValueType = typename Container::value_type;

        return std::vector<ValueType>(filtered.begin(), filtered.end());
    }


	// Original
    // Extended data processor with more sophisticated operations
    //template<NumericContainer auto container>
    //auto advancedAnalysis(const container& data) {
    //    namespace rv = std::views;

    //    auto pipeline = data
    //        | rv::filter([](auto x) { return x > 0; })
    //        | rv::transform([](auto x) { return x * x; })
    //        | rv::take(100)  // Process only first 100 positive squares
    //        | std::ranges::to<std::vector>(); // to vector is from c++23

    //    return pipeline;
    //}

    // Extended data processor with more sophisticated operations
    template<NumericContainer Container>
    auto advancedAnalysis(const Container& data)
    {
        namespace rv = std::views;

        auto pipeline = data
            | rv::filter([](auto x) { return x > 0; })
            | rv::transform([](auto x) { return x * x; })
            | rv::take(100);

        using ValueType = typename Container::value_type;

        return std::vector<ValueType>(pipeline.begin(), pipeline.end());
    }
};

int main()
{
    std::cout << "Hello World!\n";
}

