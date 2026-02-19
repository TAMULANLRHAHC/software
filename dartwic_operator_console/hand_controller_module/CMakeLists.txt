cmake_minimum_required(VERSION 3.30)
project(hand_controller_module)

set(CMAKE_CXX_STANDARD 20)

############### SOURCE FILES ###############
file(GLOB_RECURSE SRC_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE SDK_SOURCES "${PROJECT_SOURCE_DIR}/sdk/*.cpp")

### CREATE MAIN LIBRARY ###
add_library(hand_controller_module SHARED ${SRC_SOURCES} ${SDK_SOURCES})

############### INCLUDE DIRECTORIES ###############
file(GLOB subdirs
        CONFIGURE_DEPENDS
        LIST_DIRECTORIES true
        "${PROJECT_SOURCE_DIR}/include/*")

target_include_directories(hand_controller_module PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        ${subdirs})

############### NON HEADER ONLY LIBRARY INCLUDES ###############
find_package(yaml-cpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Poco REQUIRED COMPONENTS Foundation Net)


target_link_libraries(hand_controller_module PRIVATE
        yaml-cpp::yaml-cpp
        Poco::Foundation
        Poco::Net
        nlohmann_json::nlohmann_json
)

############### OUTPUT DIRECTORIES ###############
# Default to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set_target_properties(hand_controller_module PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/release/${CMAKE_BUILD_TYPE}/bin"
)

############### COPY FILES ###############
#module_config.yaml TO OUTPUT
set(CONFIG_FILE "${CMAKE_SOURCE_DIR}/src/module_config.yaml")

add_custom_command(TARGET hand_controller_module POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CONFIG_FILE}"
        "${PROJECT_SOURCE_DIR}/release/${CMAKE_BUILD_TYPE}/module_config.yaml"
        COMMENT "Copying module_config.yaml to output folder"
)
