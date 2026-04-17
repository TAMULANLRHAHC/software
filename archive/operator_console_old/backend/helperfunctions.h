//
// Created by kemptonburton on 8/30/2025.
//

#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H
#include <string>
#include <nlohmann/json_fwd.hpp>


///TIME HELPER FUNCTIONS///
//gets current time stamp of current time with microsecond resolution
std::string getCurrentTimeStampWithMicroseconds();

///OTHER///
std::string removeQuotes(const std::string& str);

int saveJsonToFile(const std::string& file_directory, const std::string& file_name, const nlohmann::json& data);

std::string roundToDecimalPlaces(const std::string& numStr, int decimalPlaces);

std::string toLower(const std::string& str);

#endif //HELPERFUNCTIONS_H
