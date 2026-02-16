//
// Created by kemptonburton on 8/29/2025.
//

#ifndef INMEMORYDATABASE_H
#define INMEMORYDATABASE_H

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <nlohmann/json_fwd.hpp>

///Main Tag Data Map, holds all tag data///
//data type of the allowable tag values
using TagValueTypes = std::variant<std::string, double>;

//the global tag data map [tag_name, value]
extern std::unordered_map<std::string, TagValueTypes> tagData;

//mutex to protect the data map
extern std::mutex tagDataMutex;


///Functions to Access Data Map///
int upsertTag(const std::string& tag_path, TagValueTypes data);
int deleteTag(const std::string& tag_path);
int insertTag(const std::string& tag_path, TagValueTypes data);

template <typename T>
T queryTag(const std::string& tag_path, const std::optional<TagValueTypes> default_value = std::nullopt);

std::unordered_map<std::string, TagValueTypes> substring_search_tag_data_map(std::string &substring, int query_count, std::unordered_map<std::string, TagValueTypes>& data, std::mutex &mutex);


///Helper Functions///
//allowed type template for the getValue function. extracts the proper value from TagValueTypes, and preforms type conversion if avaiable
template<typename T>
T getValue(const TagValueTypes &data);
nlohmann::json convertTagDataMapTypeToJson(std::unordered_map<std::string, TagValueTypes>& data);

#endif //INMEMORYDATABASE_H
