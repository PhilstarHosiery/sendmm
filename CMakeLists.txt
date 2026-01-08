cmake_minimum_required(VERSION 3.10)
project(SendMattermost VERSION 1.0)

# Set C++ Standard (C++17 for best compatibility with nlohmann/json)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the Install Path Default
# Users can override this with cmake -DCMAKE_INSTALL_PREFIX=... but this sets your default.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/storage/philstar/" CACHE PATH "Default install path" FORCE)
endif()

# Find Required Packages (CURL is standard, JSON is header-only)
find_package(CURL REQUIRED)
find_path(NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp
    PATHS /usr/local/include /usr/include /opt/local/include
    DOC "Path to nlohmann/json.hpp"
)

if(NOT NLOHMANN_JSON_INCLUDE_DIR)
    message(FATAL_ERROR "nlohmann/json header not found! Try 'pkg install nlohmann-json'")
endif()

# Define the Executable
add_executable(SendMattermost main.cpp)

# Include Directories (FreeBSD keeps headers in /usr/local/include)
target_include_directories(SendMattermost PRIVATE
    ${CURL_INCLUDE_DIRS}
    ${NLOHMANN_JSON_INCLUDE_DIR}
    /usr/local/include
)

# Link Libraries
target_link_libraries(SendMattermost PRIVATE ${CURL_LIBRARIES})

# Installation Instructions
# The binary will go to /storage/philstar/bin/SendMattermost
install(TARGETS SendMattermost DESTINATION bin)
