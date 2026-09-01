#include "ConfigManager.h"

#include <iostream>
#include <typeinfo>

void ConfigManager::loadConfig(const std::vector<std::string>& configLines) {
    for (const auto& line : configLines) {
        // Use structured bindings to destructure the parsing result cleanly.
        // The if statement initializer keeps the parsed values scoped to this block.
        if (auto [key, value] = parseConfigLine(line); key && value) {
            // Access the values held by the optional objects using operator*.
            settings[*key] = *value;
        }
    }
}

// Use structured bindings for clean parsing at the call site.
std::pair<std::optional<std::string>, std::optional<ConfigManager::ConfigValue>>
ConfigManager::parseConfigLine(const std::string& line) {
    auto pos = line.find('=');
    if (pos == std::string::npos) {
        return { std::nullopt, std::nullopt };
    }

    std::string key = line.substr(0, pos);
    std::string valueStr = line.substr(pos + 1);

    // Try parsing the value as each supported type.
    if (valueStr == "true" || valueStr == "false") {
        // Store the value as bool.
        return { key, valueStr == "true" };
    }

    if (valueStr.find('.') != std::string::npos) {
        try {
            double value = std::stod(valueStr);
            // Store the value as double.
            return { key, value };
        }
        catch (...) {
            // Ignore conversion errors so the remaining types can be tried.
        }
    }

    try {
        int value = std::stoi(valueStr);
        // Store the value as int.
        return { key, value };
    }
    catch (...) {
        // Ignore conversion errors and store the value as a string instead.
    }

    // Store the value as string when it does not match another supported type.
    return { key, valueStr };
}

void runConfigManagerBenchmark(std::shared_ptr<ConfigManager> configManager) {
    // Configuration lines.
    std::vector<std::string> configLines = {
        "database_port=5432",
        "app_name=MyApp",
        "debug_mode=true",
        "timeout=3.5"
    };

    // Load configuration.
    configManager->loadConfig(configLines);

    if (auto port = configManager->getValue<int>("database_port")) {
        std::cout << "Database port: " << *port << '\n';
        std::cout << "Type of port: " << typeid(port).name() << '\n';
        std::cout << "Type of *port: " << typeid(*port).name() << '\n';
    }

    if (auto name = configManager->getValue<std::string>("app_name")) {
        std::cout << "Application name: " << *name << '\n';
        std::cout << "Type of name: " << typeid(name).name() << '\n';
        std::cout << "Type of *name: " << typeid(*name).name() << '\n';
    }

    if (auto debug = configManager->getValue<bool>("debug_mode")) {
        std::cout << "Debug mode: "
            << (*debug ? "true" : "false") << '\n';
        std::cout << "Type of debug: " << typeid(debug).name() << '\n';
        std::cout << "Type of *debug: " << typeid(*debug).name() << '\n';
    }

    if (auto timeout = configManager->getValue<double>("timeout")) {
        std::cout << "Timeout: " << *timeout << '\n';
        std::cout << "Type of timeout: " << typeid(timeout).name() << '\n';
        std::cout << "Type of *timeout: " << typeid(*timeout).name() << '\n';
    }

    // Try to retrieve a non-existing key.
    if (auto value = configManager->getValue<int>("unknown_setting")) {
        std::cout << "Value: " << *value << '\n';
    }
    else {
        std::cout << "Setting not found\n";
    }
}
