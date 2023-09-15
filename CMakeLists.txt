cmake_minimum_required(VERSION 3.26)
project(WaifuGen)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math --static")
find_package(CURL CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
include_directories(${CURL_INCLUDE_DIR})
add_executable(WaifuGen main.cpp)

target_link_libraries(WaifuGen PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)
