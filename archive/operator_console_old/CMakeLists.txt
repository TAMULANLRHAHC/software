########## PROJECT SETTINGS ##########
cmake_minimum_required(VERSION 3.30)
#CREATE PROJECT
project(hydraulic_arm_operation_software)
#big object file limit
add_compile_options(/bigobj)
#DEFINE C++ VERSION
set(CMAKE_CXX_STANDARD 20)

#specify windows 10
add_definitions(-D_WIN32_WINNT=0x0A00)

########## TARGET #########
### GET BUILD FILES FOR EXCECUTABLE ###
#get all .cpp files in the backend directory
file(GLOB BACKEND_SOURCES "${CMAKE_SOURCE_DIR}/backend/*.cpp")

### CREATE MAIN EXCECUTABLE ###
add_executable(hydraulic_arm_operation_software ${BACKEND_SOURCES})

### INCLUDE DIRECTORIES ### (to look for header files)
target_include_directories(hydraulic_arm_operation_software PRIVATE
        "${CMAKE_SOURCE_DIR}/backend" #BACKEND CODE
)

### LIBRARY FINDING AND LINKING ###
#CROW
find_package(Crow CONFIG REQUIRED)
#NLOHMANN
find_package(nlohmann_json CONFIG REQUIRED)
#poco
find_package(Poco REQUIRED COMPONENTS Foundation Net)

#LINK LIBRARIES
target_link_libraries(hydraulic_arm_operation_software PRIVATE
        Crow::Crow
        asio::asio
        ws2_32
        wsock32
        nlohmann_json::nlohmann_json
        Poco::Foundation
        Poco::Net
)