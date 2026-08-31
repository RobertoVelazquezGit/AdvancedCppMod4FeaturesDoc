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
    typename Container::value_type;  // this is one of the requirements for a type to be considered NumericContainer  
    requires Numeric<typename Container::value_type>;
    { c.begin() } -> std::input_iterator;
    { c.end() } -> std::input_iterator;
};

// Additional concept for string processing
template<typename T>
concept StringLike = std::convertible_to<T, std::string>;

class DataProcessor {
public:
    template<NumericContainer Container>
    auto analyzeData(Container& data) {
        using ValueType = typename Container::value_type;

        // Use ranges for functional-style processing
        auto positiveValues = data
            | std::views::filter([](const auto& x) { return x > 0; });

        auto squares = positiveValues
            | std::views::transform([](const auto& x) { return x * x; });

        // Calculate statistics
		ValueType sum = std::accumulate(  // Here stops being lazy and starts being eager, because we need to calculate the sum and count   
            squares.begin(),
            squares.end(),
            ValueType{ 0 }
        );

		size_t count = std::distance(squares.begin(), squares.end());  // squares is not a container, so we need to use std::distance to get the count of elements  

        return std::make_tuple(
            sum,
            count,
            count > 0 ? sum / count : ValueType{ 0 }
        );
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
		// Classic STL algorithm std::sort(data.begin(), data.end());, std::ranges_sort with c++20,
        // and std::ranges::sort with c++20, are all valid ways to sort a vector.
        // The difference is that std::ranges::sort is a range-based algorithm that can be used with any range, not just containers.
        // It also has a more modern interface that allows for more flexibility in specifying the sorting criteria.
    }

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

        // Commented out Visual studio return std::vector<ValueType>(pipeline.begin(), pipeline.end());
        std::vector<ValueType> result;

        for (const auto& value : pipeline) {
            result.push_back(value);
        }

        return result;
    }
};

int main()
{
    DataProcessor processor;

    std::vector<int> data{ 5, -3, 8, 1, -7, 4, 10, -2 };

    // ---------------------------------------------------------
    // 1. analyzeData()
    // Filters positive values, squares them and calculates:
    // sum, count and average.
    // ---------------------------------------------------------

    auto [sum, count, average] = processor.analyzeData(data);

    std::cout << "analyzeData:\n";
    std::cout << "Sum:     " << sum << '\n';
    std::cout << "Count:   " << count << '\n';
    std::cout << "Average: " << average << "\n\n";


    // ---------------------------------------------------------
    // 2. advancedSort()
    // ---------------------------------------------------------

    auto sortedData = data;

    processor.advancedSort(sortedData);

    std::cout << "Ascending sort:\n";

    for (const auto& value : sortedData)
        std::cout << value << ' ';

    std::cout << "\n\n";


    processor.advancedSort(sortedData, false);

    std::cout << "Descending sort:\n";

    for (const auto& value : sortedData)
        std::cout << value << ' ';

    std::cout << "\n\n";


    // ---------------------------------------------------------
    // 3. filterData()
    // Keep only values greater than 3.
    // ---------------------------------------------------------

    auto filtered = processor.filterData(
        data,
        [](int x) {
            return x > 3;
        }
    );

    std::cout << "Filtered values (> 3):\n";

    for (const auto& value : filtered)
        std::cout << value << ' ';

    std::cout << "\n\n";


    // ---------------------------------------------------------
    // 4. advancedAnalysis()
    // Keeps positive values, squares them and takes
    // the first 100 results.
    // ---------------------------------------------------------

    auto result = processor.advancedAnalysis(data);

    std::cout << "Advanced analysis:\n";

    for (const auto& value : result)
        std::cout << value << ' ';

    std::cout << '\n';

    return 0;
}

