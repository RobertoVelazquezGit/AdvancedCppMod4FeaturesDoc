#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <numeric>
#include <ranges>
#include <string>
#include <tuple>
#include <vector>

// Define concepts for type constraints.
template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

// Require a type to support the < and > operators,
// with results convertible to bool.
template<typename T>
concept Comparable = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
    { a > b } -> std::convertible_to<bool>;
};

template<typename Container>
concept NumericContainer = requires(Container c) {
    typename Container::value_type;
    requires Numeric<typename Container::value_type>;
    { c.begin() } -> std::input_iterator;
    { c.end() } -> std::input_iterator;
};

// Additional concept for string processing.
template<typename T>
concept StringLike = std::convertible_to<T, std::string>;

class DataProcessor {
public:
    template<NumericContainer Container>
    auto analyzeData(Container& data) {
        using ValueType = typename Container::value_type;

        // Use ranges for functional-style processing.
        auto positiveValues = data
            | std::views::filter([](const auto& x) { return x > 0; });

        auto squares = positiveValues
            | std::views::transform([](const auto& x) { return x * x; });

        // Calculate statistics. This operation stops being lazy because the
        // values must be evaluated to calculate the sum and count.
        ValueType sum = std::accumulate(
            squares.begin(),
            squares.end(),
            ValueType{ 0 }
        );

        // squares is a view rather than a container, so use std::distance.
        std::size_t count = std::distance(squares.begin(), squares.end());

        return std::make_tuple(
            sum,
            count,
            count > 0 ? sum / count : ValueType{ 0 }
        );
    }

    // Concepts-constrained sorting function.
    template<Comparable T>
    void advancedSort(std::vector<T>& data, bool ascending = true) {
        if (ascending) {
            std::ranges::sort(data);
        }
        else {
            std::ranges::sort(data, std::greater<T>{});
        }

        // Classic std::sort, std::ranges_sort and std::ranges::sort are valid
        // ways to sort a vector. std::ranges::sort has a range-based interface
        // that also supports projections and other range types.
    }

    // Generic filter with concepts.
    template<NumericContainer Container, typename Predicate>
    auto filterData(Container& data, Predicate pred)
        requires std::predicate<Predicate, typename Container::value_type>
    {
        auto filtered = data
            | std::views::filter(pred);

        using ValueType = typename Container::value_type;

        return std::vector<ValueType>(filtered.begin(), filtered.end());
    }

    // Extended data processor with more sophisticated operations.
    template<NumericContainer Container>
    auto advancedAnalysis(const Container& data) {
        namespace rv = std::views;

        auto pipeline = data
            | rv::filter([](auto x) { return x > 0; })
            | rv::transform([](auto x) { return x * x; })
            | rv::take(100);

        using ValueType = typename Container::value_type;

        // Direct construction from the view iterators is commented out in the
        // original Visual Studio example, so copy the values explicitly.
        std::vector<ValueType> result;

        for (const auto& value : pipeline) {
            result.push_back(value);
        }

        return result;
    }
};

// Run the concepts and ranges test bench.
void runConceptsRanges(std::shared_ptr<DataProcessor> dataProcessor);
