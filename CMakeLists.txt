cmake_minimum_required(VERSION 3.10)
project(sendmm VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Default Install Path: /usr/local
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "Default install path" FORCE)
endif()

find_package(CURL REQUIRED)
find_path(NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp
    PATHS /usr/local/include /usr/include
)

if(NOT NLOHMANN_JSON_INCLUDE_DIR)
    message(FATAL_ERROR "nlohmann/json header not found! Try 'pkg install nlohmann-json'")
endif()

# Source is now in src/ directory
add_executable(sendmm src/main.cpp)

target_include_directories(sendmm PRIVATE
    ${CURL_INCLUDE_DIRS}
    ${NLOHMANN_JSON_INCLUDE_DIR}
    /usr/local/include
)

target_link_libraries(sendmm PRIVATE ${CURL_LIBRARIES})

# Install Binary to /usr/local/bin
install(TARGETS sendmm DESTINATION bin)

# Install Sample Config to /usr/local/etc/sendmm.conf.sample
install(FILES conf/sendmm.conf DESTINATION etc RENAME sendmm.conf.sample)
