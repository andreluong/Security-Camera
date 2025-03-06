cmake_minimum_required(VERSION 3.18)
project(Server VERSION 1.0 DESCRIPTION "Security Camera Server")

set(CMAKE_CXX_STANDARD 17) # NOTE: Errors with C++20 with websocket++
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Werror -Wpedantic -Wextra -D_POSIX_C_SOURCE=200809L -g)
add_compile_options(-fdiagnostics-color)

include(FetchContent)

# What folders to build
add_subdirectory(hal)  
add_subdirectory(app)
add_subdirectory(lcd)
add_subdirectory(lgpio)

# OpenCV
set(OpenCV_DIR "/usr/aarch64-linux-gnu/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)
add_library(opencv INTERFACE)
target_include_directories(opencv INTERFACE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(opencv INTERFACE ${OpenCV_LIBS})

# WebSocket++
FetchContent_Declare(websocketpp 
    GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git 
    GIT_TAG 0.8.2)
FetchContent_GetProperties(websocketpp)

if(NOT websocketpp_POPULATED)
  FetchContent_Populate(websocketpp)
  add_subdirectory(${websocketpp_SOURCE_DIR} ${websocketpp_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_library(websocketpp INTERFACE)
target_include_directories(websocketpp INTERFACE ${websocketpp_SOURCE_DIR})
