#include "ConceptsRanges.h"

#include <iostream>

void runConceptsRanges(std::shared_ptr<DataProcessor> dataProcessor) {
    std::vector<int> data{ 5, -3, 8, 1, -7, 4, 10, -2 };

    // ---------------------------------------------------------
    // 1. analyzeData()
    // Filters positive values, squares them and calculates:
    // sum, count and average.
    // ---------------------------------------------------------

    auto [sum, count, average] = dataProcessor->analyzeData(data);

    std::cout << "analyzeData:\n";
    std::cout << "Sum:     " << sum << '\n';
    std::cout << "Count:   " << count << '\n';
    std::cout << "Average: " << average << "\n\n";

    // ---------------------------------------------------------
    // 2. advancedSort()
    // ---------------------------------------------------------

    auto sortedData = data;

    dataProcessor->advancedSort(sortedData);

    std::cout << "Ascending sort:\n";

    for (const auto& value : sortedData) {
        std::cout << value << ' ';
    }

    std::cout << "\n\n";

    dataProcessor->advancedSort(sortedData, false);

    std::cout << "Descending sort:\n";

    for (const auto& value : sortedData) {
        std::cout << value << ' ';
    }

    std::cout << "\n\n";

    // ---------------------------------------------------------
    // 3. filterData()
    // Keep only values greater than 3.
    // ---------------------------------------------------------

    auto filtered = dataProcessor->filterData(
        data,
        [](int x) {
            return x > 3;
        }
    );

    std::cout << "Filtered values (> 3):\n";

    for (const auto& value : filtered) {
        std::cout << value << ' ';
    }

    std::cout << "\n\n";

    // ---------------------------------------------------------
    // 4. advancedAnalysis()
    // Keeps positive values, squares them and takes
    // the first 100 results.
    // ---------------------------------------------------------

    auto result = dataProcessor->advancedAnalysis(data);

    std::cout << "Advanced analysis:\n";

    for (const auto& value : result) {
        std::cout << value << ' ';
    }

    std::cout << '\n';
}
