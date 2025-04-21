cmake_minimum_required(VERSION 3.30)
project(custom_vector_cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(vector.h X.h test.cpp benchmark.cpp)