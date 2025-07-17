# > cmake -S . -B build
# > cmake --build build
# > ./build/main

cmake_minimum_required(VERSION 3.14)

project(Search_tree)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY build/)

add_executable(main src/main.cpp)
