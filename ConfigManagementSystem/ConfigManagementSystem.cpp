
/*
C++17 required
Build a sophisticated configuration system that uses C++17 features to handle complex application settings safely and efficiently.
Practice
In the code below, Complete the parseConfigLine method to parse configuration strings like "database_port=5432", "app_name=MyApp", "debug_mode=true".
Use structured bindings to destructure the parsing results cleanly.
Implement error handling with std::optional for invalid configuration entries.
Test with various data types and observe how if constexpr enables compile - time type checking.
*/

#include <iostream>
#include <optional>
#include <variant>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <typeinfo>

class ConfigManager {
private:
    using ConfigValue = std::variant<int, double, std::string, bool>;
    std::map<std::string, ConfigValue> settings;

public:
    // Load configuration from key-value pairs
    void loadConfig(const std::vector<std::string>& configLines) {
        for (const auto& line : configLines) {
			// Use structured bindings to destructure the parsing results cleanly
			// if with initializer to handle optional values    
            if (auto [key, value] = parseConfigLine(line); key && value) {
                settings[*key] = *value;
            }
        }
    }

    // Template method with if constexpr for type-safe retrieval
    template<typename T>
    std::optional<T> getValue(const std::string& key) const {
        auto it = settings.find(key);
        if (it == settings.end()) {
            return std::nullopt;
        }

        if constexpr (std::is_same_v<T, int>) {
            // Checks whether the variant currently holds a value of the specified type.
            if (std::holds_alternative<int>(it->second)) {
                return std::get<int>(it->second);
            }
        }
        else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(it->second)) {
                return std::get<double>(it->second);
            }
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::string>(it->second)) {
                return std::get<std::string>(it->second);
            }
        }
        else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(it->second)) {
                return std::get<bool>(it->second);
            }
        }
        return std::nullopt;
    }

private:
    // Use structured bindings for clean parsing
    std::pair<std::optional<std::string>, std::optional<ConfigValue>>
        parseConfigLine(const std::string& line) {
        auto pos = line.find('=');
        if (pos == std::string::npos) {
            return { std::nullopt, std::nullopt };
        }

        std::string key = line.substr(0, pos);
        std::string valueStr = line.substr(pos + 1);

        // Try parsing as different types
        if (valueStr == "true" || valueStr == "false") {
			return { key, valueStr == "true" };  // Store as bool
        }

        if (valueStr.find('.') != std::string::npos) {
            try {
                double value = std::stod(valueStr);
				return { key, value }; // Store as double   
            }
			catch (...) {}  // Ignore conversion errors in order to try other types next
        }

        try {
            int value = std::stoi(valueStr);
            return { key, value }; // Store as int  
        }
        catch (...) {}

        return { key, valueStr }; // Store as string    
    }
};

int main()
{
    ConfigManager config;

    // Configuration lines
    std::vector<std::string> configLines = {
        "database_port=5432",
        "app_name=MyApp",
        "debug_mode=true",
        "timeout=3.5"
    };

    // Load configuration
    config.loadConfig(configLines);

    if (auto port = config.getValue<int>("database_port")) {
        std::cout << "Database port: " << *port << '\n';
        std::cout << "Type of port: " << typeid(port).name() << '\n';
        std::cout << "Type of *port: " << typeid(*port).name() << '\n';
    }

    if (auto name = config.getValue<std::string>("app_name")) {
        std::cout << "Application name: " << *name << '\n';
        std::cout << "Type of name: " << typeid(name).name() << '\n';
        std::cout << "Type of *name: " << typeid(*name).name() << '\n';
    }

    if (auto debug = config.getValue<bool>("debug_mode")) {
        std::cout << "Debug mode: "
            << (*debug ? "true" : "false") << '\n';
        std::cout << "Type of debug: " << typeid(debug).name() << '\n';
        std::cout << "Type of *debug: " << typeid(*debug).name() << '\n';
    }

    if (auto timeout = config.getValue<double>("timeout")) {
        std::cout << "Timeout: " << *timeout << '\n';
        std::cout << "Type of timeout: " << typeid(timeout).name() << '\n';
        std::cout << "Type of *timeout: " << typeid(*timeout).name() << '\n';
    }

    // Try to retrieve a non-existing key
    if (auto value = config.getValue<int>("unknown_setting")) {
        std::cout << "Value: " << *value << '\n';
    }
    else {
        std::cout << "Setting not found\n";
    }

    return 0;
}
