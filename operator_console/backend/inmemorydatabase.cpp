//
// Created by kemptonburton on 8/29/2025.
//

#include "inmemorydatabase.h"

#include <helperfunctions.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

//INIT TAG DATA MAP AND MUTEX
std::unordered_map<std::string, TagValueTypes> tagData = {};
std::mutex tagDataMutex;

// Function to insert or update a tag value
int upsertTag(const std::string& tag_path, TagValueTypes data) {

    // Lock mutex for thread safety
    std::lock_guard<std::mutex> guard(tagDataMutex);

    //add the data
    tagData[tag_path] = data;

    // Success
    return 1;
}

int deleteTag(const std::string& tag_path) {

    // Lock mutex for thread safety
    std::lock_guard<std::mutex> guard(tagDataMutex);

    // if current value exists
    if (tagData.contains(tag_path)) {
        tagData.erase(tag_path);
        // Success - value was deleted
        return 1;
    }

    //if current value does not exist
    return -5; // tag not found
}


// Function to insert a tag value, if it does not exist.
int insertTag(const std::string& tag_path, TagValueTypes data) {
    // Lock mutex for thread safety
    std::lock_guard<std::mutex> guard(tagDataMutex);

    // only insert the value. do not update it if it already exists
    // if current value exists
    if (tagData.contains(tag_path)) {
        return -7; // value exists
        //if current value does not exist, dont attempt to cast
    } else {
        tagData[tag_path] = data;
    }

    // Success - value was inserted
    return 1;
}

// Function to query a tag and attempt to cast its value to a specific type
template double queryTag<double>(const std::string& tag_path, const std::optional<TagValueTypes> default_value);
template std::string queryTag<std::string>(const std::string& tag_path, const std::optional<TagValueTypes> default_value);

template <typename T>
T queryTag(const std::string& tag_path, const std::optional<TagValueTypes> default_value) {

    // Lock mutex for thread safety
    std::lock_guard<std::mutex> guard(tagDataMutex);

    //get tag group map
    auto& tag_group_map = tagData[tag_path];

    try {
        //If nested document field exists
        if (tagData.find(tag_path) != tagData.end()) {
            // Retrieve and cast value
            return getValue<T>(tagData[tag_path]);
        } else {
            //if default value has been provided
            if (default_value.has_value()) {
                tagData[tag_path] = default_value.value(); // insert default value
                return getValue<T>(default_value.value()); // RETURN DEFAULT VALUE
            }

            return getValue<T>(-5.0); // TAG NOT FOUND - DID NOT INSERT
        }

    } catch (const std::bad_variant_access&) {
        // If conversion fails or type mismatch, return error code
        if constexpr (std::is_same_v<T, double>) {
            return static_cast<T>(-2); // Return -2 if there's a casting error
        } else {
            return std::string("-2"); // Return "-2" for string
        }
    }
}

// Function to handle type conversion safely
template <typename T>
T getValue(const TagValueTypes& data) {
    // Try to extract the value of type T
    if (auto ptr = std::get_if<T>(&data)) {
        return *ptr; // Return the value of type T
    }

    // Attempt to convert data to type T
    try {
        if (std::holds_alternative<double>(data)) {
            double val = std::get<double>(data);
            if constexpr (std::is_same_v<T, std::string>) {
                return std::to_string(val); // Convert double to string
            }
        } else if (std::holds_alternative<std::string>(data)) {
            const std::string& val = std::get<std::string>(data);
            if constexpr (std::is_same_v<T, double>) {
                return std::stod(val); // Convert string to double
            }
        }
    } catch (...) {
        // Catch any conversion error and throw a more specific exception
        throw std::bad_variant_access();
    }

    // If no valid cast is possible, throw exception
    throw std::bad_variant_access();
}

std::unordered_map<std::string, TagValueTypes> substring_search_tag_data_map(
    std::string& substring,
    int query_count,
    std::unordered_map<std::string, TagValueTypes>& data,
    std::mutex& mutex)
{
    // Lock the mutex to ensure thread safety
    std::lock_guard<std::mutex> lock(mutex);

    // Counter to keep track of queries to keep
    int count = 0;

    // Result map for matching entries
    std::unordered_map<std::string, TagValueTypes> result;

    // Iterate over the input data map
    for (const auto& [key, value] : data) {
        // Case-insensitive substring search
        std::string key_lower = toLower(key);
        std::string substring_lower = toLower(substring);

        if (key_lower.find(substring_lower) != std::string::npos) {
            // Handle different types in TagValueTypes
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, double>) {
                    // Round double to 6 decimal places
                    std::ostringstream oss;
                    oss << std::fixed << std::setprecision(6) << val;
                    std::string rounded_str = oss.str();
                    std::istringstream iss(rounded_str);
                    double rounded_val;
                    iss >> rounded_val;
                    result[key] = rounded_val;
                } else {
                    // Non-double types are copied as-is
                    result[key] = value;
                }
            }, value);

            count++;
        }

        if (count >= query_count) {
            break;
        }
    }

    return result;
}

// Function to convert the tag data to JSON
// Define to_json for TagValueTypes
namespace nlohmann {
    void to_json(nlohmann::json& j, const TagValueTypes& value) {
        std::visit([&j](const auto& v) {
            j = v; // Converts std::string or double to JSON
        }, value);
    }
}

nlohmann::json convertTagDataMapTypeToJson(std::unordered_map<std::string, TagValueTypes>& data) {

    // Final JSON object to store key-value pairs
    nlohmann::json result;

    // Iterate over the map
    for (const auto& [key, value] : data) {
        result[key] = value; // Serialize TagValueTypes to JSON
    }

    return result;
}