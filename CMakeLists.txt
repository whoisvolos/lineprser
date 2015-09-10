cmake_minimum_required(VERSION 3.3)
project(sparser)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp LineParser.cpp LineParser.h)
add_executable(sparser ${SOURCE_FILES})