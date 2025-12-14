cmake_minimum_required(VERSION 3.22)
project(brainrot_cpp)

set(CMAKE_CXX_STANDARD 23)
find_package(SDL3 CONFIG REQUIRED)

add_executable(brainrot_cpp main.cpp)

target_link_libraries(brainrot_cpp PRIVATE SDL3::SDL3)
