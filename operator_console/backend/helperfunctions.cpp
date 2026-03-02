//
// Created by kemptonburton on 8/30/2025.
//

#include "helperfunctions.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <unordered_set>
#include <variant>
#include <nlohmann/json.hpp>


int createDirectory(const std::string& path) {
    // check if project directory already exists
    if (std::filesystem::exists(path)) {
        //already exists
        return -1;
    } else {
        // Try to create the directory
        if (std::filesystem::create_directory(path)) {
            //created successfullly
            return 1;
        } else {
            //failed to create
            return -2;
        }
    }
}

//gets a time stamp in microseconds
std::string getCurrentTimeStampWithMicroseconds() {
    // Get the current time point with high precision
    auto now = std::chrono::system_clock::now();

    // Convert it to time since epoch (in microseconds)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());

    // Get the number of microseconds since the epoch
    long long microseconds_since_epoch = duration.count();

    // Get the current time using system_clock for easier formatting
    auto system_now = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(system_now);

    // Create a stringstream to format the time and microseconds
    std::stringstream time_stream;
    time_stream << std::put_time(std::localtime(&current_time), "%Y-%m-%d %H:%M:%S") << "."
                << std::setw(6) << std::setfill('0') << (microseconds_since_epoch % 1000000);

    // Return the formatted time as a string
    return time_stream.str();
}

std::string removeQuotes(const std::string &str) {
    if (str.length() >= 2 && str.front() == '"' && str.back() == '"') {
        return str.substr(1, str.length() - 2); // Remove first and last character
    }
    return str; // Return original string if no quotes
}

int saveJsonToFile(const std::string& file_directory, const std::string& file_name, const nlohmann::json& data) {

    // Ensure the directory exists
    if (!std::filesystem::exists(file_directory)) {
        if (!std::filesystem::create_directories(file_directory)) {
            return -1;
        }
    }

    // Specify the output file
    std::string file_path = file_directory+"/"+file_name;
    std::ofstream outFile(file_path);

    // Check if the file is open
    if (!outFile.is_open()) {
        //file is open
        return -2;
    }

    try {
        // Write the JSON object to the file
        outFile << data.dump(4); // Pretty print with 4 spaces indentation
        // Close the file
        outFile.close();
    }
    catch (...) {
        return -3;
    }

    //success
    return 1;
}

std::variant<int, nlohmann::json>  readJsonFile(const std::string& file_directory, const std::string& file_name) {
    std::string filePath = file_directory+"/"+file_name;

    // Declare a JSON object
    nlohmann::json jsonData;

    // Read the JSON file
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        return -1; //failed to open file
    }

    try {
        // Parse the JSON file into the jsonData object
        inFile >> jsonData;
    } catch (const nlohmann::json::parse_error& e) {
        return -2; //faled to read json
    }

    // Close the file
    inFile.close();

    return jsonData;
}

std::string convert_timestamp_to_microseconds_epoch(const std::string& timestamp) {
    // Parse the input timestamp
    std::tm tm = {};
    std::stringstream ss(timestamp.substr(0, 19)); // Extract "2025-01-15 18:09:26"
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        throw std::runtime_error("Failed to parse timestamp.");
    }

    // Parse the microseconds part
    int microseconds = std::stoi(timestamp.substr(20));

    // Convert the time to a time_point
    auto time_point = std::chrono::system_clock::from_time_t(std::mktime(&tm));

    // Add the microseconds part
    time_point += std::chrono::microseconds(microseconds);

    // Calculate microseconds since epoch
    auto duration = time_point.time_since_epoch();
    long long microseconds_epoch = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();

    // Convert to string and return
    return std::to_string(microseconds_epoch);
}

std::string roundToDecimalPlaces(const std::string& numStr, int decimalPlaces) {
    double number;
    size_t pos;

    // Try to convert to double
    try {
        number = std::stod(numStr, &pos);
        if (pos != numStr.length()) {
            return numStr;  // Return original if there are extra characters
        }
    } catch (const std::invalid_argument&) {
        return numStr;  // Return original if not a valid number
    } catch (const std::out_of_range&) {
        return numStr;  // Return original if out of range
    }

    // Count current decimal places and check for significant digits
    size_t decimalPos = numStr.find('.');
    if (decimalPos == std::string::npos) {
        return numStr;  // No decimal point, return as is (e.g., "3")
    }

    std::string decimalPart = numStr.substr(decimalPos + 1);
    int currentDecimalCount = decimalPart.length();

    // Check if there are significant digits beyond decimalPlaces
    bool hasSignificantDigits = false;
    if (currentDecimalCount > decimalPlaces) {
        std::string digitsAfter = decimalPart.substr(decimalPlaces);
        for (char c : digitsAfter) {
            if (c != '0') {
                hasSignificantDigits = true;
                break;
            }
        }
    }

    if (hasSignificantDigits) {
        // Round to specified decimal places
        double multiplier = std::pow(10.0, decimalPlaces);
        double rounded = std::round(number * multiplier) / multiplier;

        std::string result = std::to_string(rounded);
        size_t resultDecimalPos = result.find('.');
        if (resultDecimalPos != std::string::npos) {
            size_t desiredLength = resultDecimalPos + decimalPlaces + 1;
            if (result.length() > desiredLength) {
                result = result.substr(0, desiredLength);
            } else {
                while (result.length() < desiredLength) {
                    result += '0';
                }
            }
        } else {
            result += '.';
            for (int i = 0; i < decimalPlaces; i++) {
                result += '0';
            }
        }
        return result;
    } else {
        // No significant digits beyond decimalPlaces, truncate trailing zeros
        std::string result = numStr;
        size_t lastNonZero = result.find_last_not_of('0');
        if (lastNonZero != std::string::npos) {
            if (result[lastNonZero] == '.') {
                result = result.substr(0, lastNonZero);  // Remove decimal point if no decimals
            } else {
                result = result.substr(0, lastNonZero + 1);  // Keep up to last non-zero
            }
        }
        return result;
    }
}

// Helper function to convert string to lowercase
std::string toLower(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}