cmake_minimum_required(VERSION 3.20)
project(cpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(cpp main.cpp test/ChiSquaredTest.cpp)
