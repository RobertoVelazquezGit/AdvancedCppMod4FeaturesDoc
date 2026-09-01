#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

class ConfigManager {
private:
    using ConfigValue = std::variant<int, double, std::string, bool>;

    // A map does not allow duplicate keys, so it inherently prevents duplicate entries.
    std::map<std::string, ConfigValue> settings;

public:
    // Load configuration from key-value pairs.
    void loadConfig(const std::vector<std::string>& configLines);

    // Template method with if constexpr for type-safe retrieval.
    template<typename T>
    std::optional<T> getValue(const std::string& key) const {
        auto it = settings.find(key);
        if (it == settings.end()) {
            return std::nullopt;
        }

        if constexpr (
            std::is_same_v<T, int> ||
            std::is_same_v<T, double> ||
            std::is_same_v<T, std::string> ||
            std::is_same_v<T, bool>) {
            // Check whether the variant currently holds a value of the requested type.
            if (std::holds_alternative<T>(it->second)) {
                // Access the variant value using std::get<T> and return it as std::optional<T>.
                return std::get<T>(it->second);
            }
        }

        return std::nullopt;
    }

private:
    // Return optional values so invalid configuration entries can be represented safely.
    std::pair<std::optional<std::string>, std::optional<ConfigValue>>
        parseConfigLine(const std::string& line);
};

// Run the ConfigManager test bench.
void runConfigManagerBenchmark(std::shared_ptr<ConfigManager> configManager);
