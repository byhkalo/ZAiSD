cmake_minimum_required(VERSION 3.12)
project(ZAiSD_Lab5)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_COMPILER /usr/local/bin/g++-8)
set(CMAKE_C_COMPILER /usr/local/bin/gcc-8)
# enable openmp using -fopenmp
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fopenmp")

add_executable(ZAiSD_Lab5 main.cpp)